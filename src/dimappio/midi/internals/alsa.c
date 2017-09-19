#include "dimappio/midi/internals/alsa.h"
#ifdef __linux__

/**
 * Stop Flag.
 * Flip to 1 to exit.
 */
static volatile sig_atomic_t stop = 0;

/**
 * Sets the stop flag to true,
 * which is monitored by a fd loop.
 */
static void sighandler() { stop = 1; }

static void update_node_list(dm_key_node** tail, int note_on, int midi);
static void check_snd(char* desc, int err);

dm_devices* dma_get_devices() {
    dm_devices* devices = malloc(sizeof(dm_devices));
    devices->store = malloc(sizeof(dm_device) * 64);
    devices->count = 0;

    snd_seq_t* seq = NULL;
    dma_init_sequencer(&seq, "device-query");

    snd_seq_client_info_t* cinfo;
    snd_seq_port_info_t* pinfo;

    snd_seq_client_info_alloca(&cinfo);
    snd_seq_port_info_alloca(&pinfo);
    snd_seq_client_info_set_client(cinfo, -1);

    while (snd_seq_query_next_client(seq, cinfo) >= 0) {

        int client_id = snd_seq_client_info_get_client(cinfo);
        /* printf("found client: %d\n", client_id); */

        snd_seq_port_info_set_client(pinfo, client_id);
        snd_seq_port_info_set_port(pinfo, -1);

        while (snd_seq_query_next_port(seq, pinfo) >= 0) {
            int port_id = snd_seq_port_info_get_port(pinfo);

            dm_device* dev = malloc(sizeof(dm_device));

            const char* pinfo_name = snd_seq_port_info_get_name(pinfo);
            dev->name = strdup(pinfo_name);
            // dev->name = pinfo_name;

            dev->client = client_id;
            dev->port = port_id;
            // dev->name = NULL;

            devices->store[devices->count] = dev;
            devices->count++;
        }
    }

    snd_seq_close(seq);
    snd_config_update_free_global();

    // downsize to accurate storage size.
    devices->store =
        realloc(devices->store, sizeof(dm_device) * devices->count);

    return devices;
}

bool dma_client_exists(char* client_with_port) {
    dm_device* dev = dm_parse_device(client_with_port);
    snd_seq_port_info_t* port = dma_get_port_info(dev);
    free(dev);

    bool exists = (port != NULL);
    snd_seq_port_info_free(port);

    return exists;
}

snd_seq_port_info_t* dma_get_port_info(dm_device* dev) {

    snd_seq_t* seq;
    dma_init_sequencer(&seq, NULL);

    snd_seq_port_info_t* pinfo;
    snd_seq_port_info_malloc(&pinfo);

    int found = snd_seq_get_any_port_info(seq, dev->client, dev->port, pinfo);

    snd_seq_close(seq);
    snd_config_update_free_global();

    if (found == 0) {
        return pinfo;

    } else {
        snd_seq_port_info_free(pinfo);
        ub_debug("Client(%d) port not found(%d)", dev->client, dev->port);
        return NULL;
    }
}

void dma_monitor_device(dm_options* options) {
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);

    ub_debug("alsa: dma_monitor_device: setting up src\n");
    dm_device* src = dm_parse_device(options->source);
    options->source = NULL;

    ub_debug("alsa: dma_monitor_device: attempting to query port_info\n");
    snd_seq_port_info_t* pinfo = dma_get_port_info(src);
    const char* pname = snd_seq_port_info_get_name(pinfo);

    printf("Monitoring: %s [%d:%d]\n\n", pname, src->client, src->port);
    dm_midi_output* output = dma_midi_output_create(src->client, src->port);

    if (options->target != NULL) {
        ub_debug("alsa: dma_monitor_device: setting up receiver\n");
        dm_device* receiver = dm_parse_device(options->target);
        options->target = NULL;
        dma_send_events_to(output, receiver->client, receiver->port);
        free(receiver);
    }

    ub_debug("alsa: dma_monitor_device: attempting to enter event loop\n");

    snd_seq_port_info_free(pinfo);
    free(src);

    dma_event_loop(options, output, &dm_monitor_render, NULL);
    dm_output_free(output);
}

void dma_event_loop(dm_options* options, dm_midi_output* output,
                    void (*render_callback)(dm_options* options,
                                            dm_key_node* tail,
                                            dm_key_set* key_set),
                    void (*key_callback)(int key, int ch, int vel, int on)) {

    int pfds_num =
        snd_seq_poll_descriptors_count(output->dev, POLLIN | POLLOUT);
    struct pollfd* pfds = malloc(pfds_num * sizeof(*pfds));

    dm_key_node* list = dm_key_node_head();
    dm_key_group* grp = NULL;
    dm_key_set* dsts_set = dm_key_set_create(0);

    snd_seq_event_t* event = NULL;

    int midi = 0;
    int type = 0;
    int chan = 0;
    int vel = 0;
    int note_on = 0;
    int err = 0;
    int process_event = 1;

    if (render_callback != NULL) {
        render_callback(options, list, dsts_set);
    }

    for (;;) {
        // gather poll descriptors for this sequencer
        snd_seq_poll_descriptors(output->dev, pfds, pfds_num, POLLIN);

        // wait on sequencer events
        if (poll(pfds, pfds_num, -1) < 0) {
            break;
        }

        // iterate over available events
        do {
            process_event = 1;
            event = NULL;
            grp = NULL;

            err = snd_seq_event_input(output->dev, &event);

            if (err < 0) {
                break;
            }

            if (event) {
                type = event->type;
                midi = event->data.note.note;
                chan = event->data.note.channel;
                vel = event->data.note.velocity;

                note_on = (type == SND_SEQ_EVENT_NOTEON) ? 1 : 0;

                update_node_list(&list, note_on, midi);

                process_event =
                    dm_event_process(key_callback, output, options, list,
                                     &dsts_set, midi, chan, vel, note_on);

                if (process_event) {
                    if (key_callback != NULL) {
                        key_callback(midi, chan, vel, note_on);
                    }

                    dma_send_event(output, event);
                }
            }

        } while (err > 0);

        if (render_callback != NULL) {
            render_callback(options, list, dsts_set);
        }

        // Caught a sig signal, time to exit!
        if (stop) {
            break;
        }
    }

    // dm_output_free(output);
    dm_options_free(options);
    dm_keylets_free(dsts_set->keys, dsts_set->count);

    // free(dsts_set->keys);
    free(dsts_set);
    free(list);
    free(grp);
    free(pfds);
}

void dma_send_midi_note(int client, int port, char* note, bool on, int ch,
                        int vel) {
    dm_midi_output* output = dma_midi_output_create(client, port);
    dma_send_events_to(output, client, port);

    int midi = atoi(note);
    dma_send_midi(output, midi, on, ch, vel);

    dm_output_free(output);

    // TODO: close out_port since this is one-shot api
}

void dma_receive_events_from(dm_midi_output* output, int client, int port) {
    unsigned int port_caps = SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_WRITE |
                             SND_SEQ_PORT_CAP_SUBS_READ |
                             SND_SEQ_PORT_CAP_SUBS_WRITE;

    unsigned int port_type =
        SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION |
        SND_SEQ_PORT_TYPE_MIDI_GM | SND_SEQ_PORT_TYPE_HARDWARE;

    int in_port =
        dma_create_port(output->dev, "mm-input", port_caps, port_type);

    output->in_ports[output->out_count] = in_port;
    output->in_count++;

    if (in_port < 0) {
        fprintf(stderr, "Error creating sequencer port.\n");
        exit(1);

    } else {
        int err = snd_seq_connect_from(output->dev, in_port, client, port);
        if (err < 0) {
            check_snd("snd_seq_connect_from", err);
        }
    }
}
void dma_send_events_to(dm_midi_output* output, int client, int port) {
    unsigned int port_caps = SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_WRITE |
                             SND_SEQ_PORT_CAP_SUBS_READ |
                             SND_SEQ_PORT_CAP_SUBS_WRITE;

    unsigned int port_type =
        SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION |
        SND_SEQ_PORT_TYPE_MIDI_GM | SND_SEQ_PORT_TYPE_HARDWARE;

    int driver_port =
        dma_create_port(output->dev, "mm-output", port_caps, port_type);

    output->out_ports[output->out_count] = driver_port;
    output->out_count++;

    if (driver_port < 0) {
        fprintf(stderr, "Error creating sequencer port.\n");
        exit(1);

    } else {
        int err = snd_seq_connect_to(output->dev, driver_port, client, port);
        if (err < 0) {
            check_snd("snd_seq_connect_from", err);
        }
    }
}

dm_midi_output* dma_midi_output_create(int input_client, int input_port) {
    dm_midi_output* output = malloc(sizeof(dm_midi_output));
    output->dev = NULL;

    output->in_count = 0;
    output->out_count = 0;

    output->out_ports = malloc(sizeof(int*) * 64);
    output->in_ports = malloc(sizeof(int*) * 64);

    output->id = dma_init_sequencer(&output->dev, "midi-mapper");
    dma_receive_events_from(output, input_client, input_port);

    return output;
}

int dma_init_sequencer(snd_seq_t** seq, char* name) {
    int status;
    if ((status = snd_seq_open(seq, "default", SND_SEQ_OPEN_DUPLEX, 0)) < 0) {
        ub_error("Could not open sequencer: %s", snd_strerror(status));
        exit(EXIT_FAILURE);
    }

    if (name != NULL) {
        snd_seq_set_client_name(*seq, name);
    }

    snd_seq_nonblock(*seq, 1);

    int id = snd_seq_client_id(*seq);

    // free cached memory (in regards to valgrind)
    snd_config_update_free_global();

    return id;
}

int dma_create_port(snd_seq_t* seq, char* name, unsigned caps, unsigned type) {
    int port_result = snd_seq_create_simple_port(seq, name, caps, type);

    return port_result;
}

void dma_send_midi(dm_midi_output* output, int midi, bool on, int ch, int vel) {
    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);

    ev.type = on == true ? SND_SEQ_EVENT_NOTEON : SND_SEQ_EVENT_NOTEOFF;
    ev.data.control.channel = ch;
    ev.data.note.note = midi;
    ev.data.note.velocity = vel;

    dma_send_event(output, &ev);
}

void dma_send_event(dm_midi_output* output, snd_seq_event_t* ev) {

    // publish to any subscribers to the sequencer
    snd_seq_ev_set_subs(ev);

    // set output to direct
    snd_seq_ev_set_direct(ev);

    int port_count = 0;
    int* ports = NULL;

    if (output->out_count == 0) {
        port_count = output->in_count;
        ports = output->in_ports;
    } else {
        port_count = output->out_count;
        ports = output->out_ports;
    }

    for (int i = 0; i < port_count; ++i) {
        /* printf("\nsend_event[%d] -> %d\n", ports[i], ev->data.note.note); */

        // set event source
        snd_seq_ev_set_source(ev, ports[i]);

        // output event immediately
        snd_seq_event_output_direct(output->dev, ev);
    }

    snd_seq_free_event(ev);
}

static void update_node_list(dm_key_node** tail, int note_on, int midi) {
    dm_key_node* node = NULL;

    if (note_on) {
        node = NULL;
        node = dm_key_node_search(tail, midi);

        if (node == NULL) {
            // Create the new node and set it up
            node = dm_key_node_create(midi);

            // Insert the new into the list by adjoining with the tail.
            dm_key_node_insert(tail, node);
        }

    } else {
        node = dm_key_node_search(tail, midi);

        if (node != NULL) {
            dm_key_node_remove(tail, node);
        }
    }
}

static void check_snd(char* desc, int err) {
    if (err < 0) {
        ub_error("Alsa Subsystem ub_error: failed to %s because %s", desc,
                 snd_strerror(err));
        exit(EXIT_FAILURE);
    }
}
#endif
