#include "midi/internals/alsa.h"
#ifdef __linux__

Device* MMAlsa_CreateVirtualDevice(char* name) {
    /* printw("Creating virtual device: alsa\n"); */
    Device* dev;
    return dev;
}

Devices* MMAlsa_GetDevices() {
    Devices* devices = malloc(sizeof(Devices));
    devices->count = 0;

    int card = -1;
    int status = 0;

    if ((status = snd_card_next(&card)) < 0) {
        error("Failed to retrieve midi data: %s", snd_strerror(status));
    }

    if (card < 0) {
        error("No available sound cards found");
        return devices;
    }

    snd_ctl_t* ctl;

    while (card >= 0) {
        rawmidi_devices_on_card(ctl, card);

        if ((status = snd_card_next(&card)) < 0) {
            error("No other cards available: %s", snd_strerror(status));
            break;
        }
    }

    return devices;
}

/**
 * Stop Flag.
 * Flip to 1 to exit.
 */
static volatile sig_atomic_t stop = 0;

/**
 * Sets the stop flag to true,
 * which is monitored by a fd loop.
 */
void sighandler() { stop = 1; }

void MMAlsa_MonitorDevice(char* client_with_port) {
    snd_seq_t* seq;
    init_sequencer(&seq, "midimap-monitor");
    snd_seq_nonblock(seq, 1);

    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);

    const MIDIPortInfo* pinfo = MMAlsa_GetPortInfo(client_with_port);
    const MIDIAddr* addr = snd_seq_port_info_get_addr(pinfo);
    const char* pname = snd_seq_port_info_get_name(pinfo);
    const ClientPort * cp = parseStringToClientPort(client_with_port);

    create_ports(seq);
    connect_ports(seq, cp);

    printf("Monitoring: %s [%d:%d]\n", pname, addr->client, addr->port);

    int err;
    int pfds_num = snd_seq_poll_descriptors_count(seq, POLLIN);
    struct pollfd* pfds = malloc(pfds_num * sizeof(*pfds));

    for (;;) {

        // gather poll descriptors for this sequencer
        snd_seq_poll_descriptors(seq, pfds, pfds_num, POLLIN);
        fflush(stdout);

        // wait on sequencer events
        if (poll(pfds, pfds_num, -1) < 0) {
            error("poll failed");
            break;
        }


        // iterate over available events
        do {
            MIDIEvent* event;
            err = snd_seq_event_input(seq, &event);

            if (err < 0) {
                break;
            }

            if (event) {
                process_event(event);
            }

        } while (err > 0);

        if (stop) {
            break;
        }
    }

    snd_seq_close(seq);
}

static void process_event(MIDIEvent* ev) {
  char note[] = "NOTE";

  printf("[%3d:%2d ] ",
         ev->source.client,
         ev->source.port);

	switch (ev->type) {
	case SND_SEQ_EVENT_NOTEON:
		if (ev->data.note.velocity)
			printf("Note on                %2d, note %d, velocity %d\n",
			       ev->data.note.channel, ev->data.note.note, ev->data.note.velocity);
		else
			printf("Note off               %2d, note %d\n",
			       ev->data.note.channel, ev->data.note.note);
		break;
	case SND_SEQ_EVENT_NOTEOFF:
		printf("Note off               %2d, note %d, velocity %d\n",
		       ev->data.note.channel, ev->data.note.note, ev->data.note.velocity);
		break;
	case SND_SEQ_EVENT_KEYPRESS:
		printf("Polyphonic aftertouch  %2d, note %d, value %d\n",
		       ev->data.note.channel, ev->data.note.note, ev->data.note.velocity);
		break;
	case SND_SEQ_EVENT_CONTROLLER:
		printf("Control change         %2d, controller %d, value %d\n",
		       ev->data.control.channel, ev->data.control.param, ev->data.control.value);
		break;
	case SND_SEQ_EVENT_PGMCHANGE:
		printf("Program change         %2d, program %d\n",
		       ev->data.control.channel, ev->data.control.value);
		break;
	case SND_SEQ_EVENT_CHANPRESS:
		printf("Channel aftertouch     %2d, value %d\n",
		       ev->data.control.channel, ev->data.control.value);
		break;
	case SND_SEQ_EVENT_PITCHBEND:
		printf("Pitch bend             %2d, value %d\n",
		       ev->data.control.channel, ev->data.control.value);
		break;
	case SND_SEQ_EVENT_CONTROL14:
		printf("Control change         %2d, controller %d, value %5d\n",
		       ev->data.control.channel, ev->data.control.param, ev->data.control.value);
		break;
	case SND_SEQ_EVENT_NONREGPARAM:
		printf("Non-reg. parameter     %2d, parameter %d, value %d\n",
		       ev->data.control.channel, ev->data.control.param, ev->data.control.value);
		break;
	case SND_SEQ_EVENT_REGPARAM:
		printf("Reg. parameter         %2d, parameter %d, value %d\n",
		       ev->data.control.channel, ev->data.control.param, ev->data.control.value);
		break;
	case SND_SEQ_EVENT_SONGPOS:
		printf("Song position pointer      value %d\n",
		       ev->data.control.value);
		break;
	case SND_SEQ_EVENT_SONGSEL:
		printf("Song select                value %d\n",
		       ev->data.control.value);
		break;
	case SND_SEQ_EVENT_QFRAME:
		printf("MTC quarter frame          %02xh\n",
		       ev->data.control.value);
		break;
	case SND_SEQ_EVENT_TIMESIGN:
		// XXX how is this encoded?
		printf("SMF time signature         (%#010x)\n",
		       ev->data.control.value);
		break;
	case SND_SEQ_EVENT_KEYSIGN:
		// XXX how is this encoded?
		printf("SMF key signature          (%#010x)\n",
		       ev->data.control.value);
		break;
	case SND_SEQ_EVENT_START:
		if (ev->source.client == SND_SEQ_CLIENT_SYSTEM &&
		    ev->source.port == SND_SEQ_PORT_SYSTEM_TIMER)
			printf("Queue start                queue %d\n",
			       ev->data.queue.queue);
		else
			printf("Start\n");
		break;
	case SND_SEQ_EVENT_CONTINUE:
		if (ev->source.client == SND_SEQ_CLIENT_SYSTEM &&
		    ev->source.port == SND_SEQ_PORT_SYSTEM_TIMER)
			printf("Queue continue             queue %d\n",
			       ev->data.queue.queue);
		else
			printf("Continue\n");
		break;
	case SND_SEQ_EVENT_STOP:
		if (ev->source.client == SND_SEQ_CLIENT_SYSTEM &&
		    ev->source.port == SND_SEQ_PORT_SYSTEM_TIMER)
			printf("Queue stop                 queue %d\n",
			       ev->data.queue.queue);
		else
			printf("Stop\n");
		break;
	case SND_SEQ_EVENT_SETPOS_TICK:
		printf("Set tick queue pos.        queue %d\n", ev->data.queue.queue);
		break;
	case SND_SEQ_EVENT_SETPOS_TIME:
		printf("Set rt queue pos.          queue %d\n", ev->data.queue.queue);
		break;
	case SND_SEQ_EVENT_TEMPO:
		printf("Set queue tempo            queue %d\n", ev->data.queue.queue);
		break;
	case SND_SEQ_EVENT_CLOCK:
		printf("Clock\n");
		break;
	case SND_SEQ_EVENT_TICK:
		printf("Tick\n");
		break;
	case SND_SEQ_EVENT_QUEUE_SKEW:
		printf("Queue timer skew           queue %d\n", ev->data.queue.queue);
		break;
	case SND_SEQ_EVENT_TUNE_REQUEST:
		printf("Tune request\n");
		break;
	case SND_SEQ_EVENT_RESET:
		printf("Reset\n");
		break;
	case SND_SEQ_EVENT_SENSING:
		printf("Active Sensing\n");
		break;
	case SND_SEQ_EVENT_CLIENT_START:
		printf("Client start               client %d\n",
		       ev->data.addr.client);
		break;
	case SND_SEQ_EVENT_CLIENT_EXIT:
		printf("Client exit                client %d\n",
		       ev->data.addr.client);
		break;
	case SND_SEQ_EVENT_CLIENT_CHANGE:
		printf("Client changed             client %d\n",
		       ev->data.addr.client);
		break;
	case SND_SEQ_EVENT_PORT_START:
		printf("Port start                 %d:%d\n",
		       ev->data.addr.client, ev->data.addr.port);
		break;
	case SND_SEQ_EVENT_PORT_EXIT:
		printf("Port exit                  %d:%d\n",
		       ev->data.addr.client, ev->data.addr.port);
		break;
	case SND_SEQ_EVENT_PORT_CHANGE:
		printf("Port changed               %d:%d\n",
		       ev->data.addr.client, ev->data.addr.port);
		break;
	case SND_SEQ_EVENT_PORT_SUBSCRIBED:
		printf("Port subscribed            %d:%d -> %d:%d\n",
		       ev->data.connect.sender.client, ev->data.connect.sender.port,
		       ev->data.connect.dest.client, ev->data.connect.dest.port);
		break;
	case SND_SEQ_EVENT_PORT_UNSUBSCRIBED:
		printf("Port unsubscribed          %d:%d -> %d:%d\n",
		       ev->data.connect.sender.client, ev->data.connect.sender.port,
		       ev->data.connect.dest.client, ev->data.connect.dest.port);
		break;
	case SND_SEQ_EVENT_SYSEX:
		{
			unsigned int i;
			printf("System exclusive          ");
			for (i = 0; i < ev->data.ext.len; ++i)
				printf(" %02X", ((unsigned char*)ev->data.ext.ptr)[i]);
			printf("\n");
		}
		break;
	default:
		printf("Event type %d\n",  ev->type);
	}
}

bool MMAlsa_ClientExists(char* client_with_port) {
    return (MMAlsa_GetPortInfo(client_with_port) != NULL);
}

MIDIPortInfo* MMAlsa_GetPortInfo(char* client_with_port) {
    ClientPort* cp = parseStringToClientPort(client_with_port);

    snd_seq_t* seq;
    init_sequencer(&seq, NULL);

    MIDIPortInfo* pinfo;
    snd_seq_port_info_malloc(&pinfo);

    int found = snd_seq_get_any_port_info(seq, cp->client, cp->port, pinfo);

    if (found == 0) {
        return pinfo;

    } else {
        pdebug("Client(%d) port not found(%d)", cp->client, cp->port);
        return NULL;
    }
}

void MMAlsa_ClientDetails(MIDIClient* client) {
    char* type = "unknown";
    if (client->type == SND_SEQ_USER_CLIENT) {
        type = "user";

    } else if (client->type == SND_SEQ_KERNEL_CLIENT) {
        type = "kernel";
    }

    MIDIClientPort* port;
    char* caps;
    char* types;

    for (int i = 0; i < client->num_ports; i++) {
        port = client->ports[i];
        types = char_port_types(port->type);
        printf("[%d:%d]: %s (ch: %d): [%s: %s]\n", client->client_id,
               port->port_id, port->name, port->channels, type, types);

        caps = char_port_capabilities(port->capability);
        printf("    • %s\n", caps);

        printf("\n");
    }
}

MIDIClients* MMAlsa_GetClients(snd_seq_t* seq) {
    bool created_seq = false;
    if (seq == NULL) {
        created_seq = true;
        init_sequencer(&seq, "midimap-gc");
    }

    MIDIClients* clients = malloc(sizeof(MIDIClients));

    // TODO: remove this hard limit and make dynamic via realloc
    clients->store = malloc(32 * sizeof(MIDIClient*));
    clients->count = 0;

    snd_seq_client_info_t* cinfo;
    snd_seq_port_info_t* pinfo;
    MIDIClient* client;
    MIDIClientPort* port;

    snd_seq_client_info_alloca(&cinfo);
    snd_seq_port_info_alloca(&pinfo);

    snd_seq_client_info_set_client(cinfo, -1);

    int port_count;
    int client_id;
    int count = 0;

    while (snd_seq_query_next_client(seq, cinfo) >= 0) {

        clients->count++;

        /////////////////////////////////////////
        // Create a MIDIClient to start attaching new query results to.
        client = malloc(sizeof(MIDIClient));
        client->name = strdup(snd_seq_client_info_get_name(cinfo));

        // TODO: dynamically realloc port space
        client->ports = malloc(16 * sizeof(MIDIClientPort*));

        /////////////////////////////////////////
        // Reset query info per parent iteration

        // 1. Get client_id from client_info container
        client_id = snd_seq_client_info_get_client(cinfo);

        // 2. set the client_id for this client_port info
        snd_seq_port_info_set_client(pinfo, client_id);

        // 3. Set the port info to start the searching
        snd_seq_port_info_set_port(pinfo, -1);

        ///////////////////////////////////////////
        // Iterate ports for accounting
        port_count = 0;
        while (snd_seq_query_next_port(seq, pinfo) >= 0) {
            port = malloc(sizeof(MIDIClientPort));
            port->name = strdup(snd_seq_port_info_get_name(pinfo));
            port->addr = snd_seq_port_info_get_addr(pinfo);
            port->type = snd_seq_port_info_get_type(pinfo);
            port->port_id = snd_seq_port_info_get_port(pinfo);
            port->channels = snd_seq_port_info_get_midi_channels(pinfo);
            port->capability = snd_seq_port_info_get_capability(pinfo);

            client->ports[port_count] = port;
            port_count++;
        }

        client->num_ports = port_count;
        client->client_id = client_id;
        client->card = snd_seq_client_info_get_card(cinfo);
        client->pid = snd_seq_client_info_get_pid(cinfo);
        client->type = snd_seq_client_info_get_type(cinfo);

        // Assign the client data to the collection
        clients->store[count] = client;

        count++;
    }

    if (created_seq) {
        snd_seq_close(seq);
    }

    return clients;
}

static void rawmidi_devices_on_card(snd_ctl_t* ctl, int card) {
    int device = -1;
    char name[32];
    int status;

    sprintf(name, "hw:%d", card);

    if ((status = snd_ctl_open(&ctl, name, 0)) < 0) {
        error("Could not open ctl for sound card: %s\n", name);

    } else {
        /* printf("snd_ctl_opened: %s\n", name); */
        do {
            status = snd_ctl_rawmidi_next_device(ctl, &device);

            if (status < 0) {
                error("failed to determined device number\n");
                break;
            }

            if (device >= 0) {
                rawmidi_subdevice_info(ctl, card, device);

            } else {
                /* printf("unable to open rawmidi for device: %s %d\n", */
                /*        name, device); */
            }
        } while (device >= 0);
    }
}

static void rawmidi_subdevice_info(snd_ctl_t* ctl, int card, int device) {
    pdebug("reading subdevice info");
    snd_rawmidi_info_t* info;

    const char* name;
    const char* sub_name;

    int subs, subs_in, subs_out;
    int sub, in, out;
    int status;

    // prepare info for subs INPUT query
    snd_rawmidi_info_alloca(&info);
    snd_rawmidi_info_set_device(info, device);
    snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);

    snd_ctl_rawmidi_info(ctl, info);
    subs_in = snd_rawmidi_info_get_subdevices_count(info);

    // prepare info for subs OUTPUT query
    snd_rawmidi_info_alloca(&info);
    snd_rawmidi_info_set_device(info, device);
    snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);

    snd_ctl_rawmidi_info(ctl, info);
    subs_out = snd_rawmidi_info_get_subdevices_count(info);

    pdebug("hw:%d -> subs_in: %d\n", card, subs_in);
    pdebug("hw:%d -> subs_out: %d\n", card, subs_out);

    // determine the upper boundary for info searching
    subs = subs_in > subs_out ? subs_in : subs_out;

    sub = 0;
    in = out = 0;

    if ((status = is_output(ctl, card, device, sub)) < 0) {
        // is_output failed with a status code
        error("cannot get rawmidi sub output information %d:%d %s", card,
              device, snd_strerror(status));
        return;
    } else if (status) {
        // is_output determined the sub is an output
        out = 1;
    }

    // if is_output did not determine outputness, try is_input
    if (status) {
        if ((status = is_input(ctl, card, device, sub)) < 0) {
            // is_output failed with a status code
            error("cannot get rawmidi sub input information %d:%d %s", card,
                  device, snd_strerror(status));
            return;
        } else if (status) {
            // is_output determined the sub is an output
            in = 1;
        }
    }

    pdebug("hw:%d -> in: %d / out: %d", card, in, out);

    // still no luck, we need to return..
    if (status == 0) {
        error("Could not determine any subputs");
        return;
    }

    name = snd_rawmidi_info_get_name(info);
    sub_name = snd_rawmidi_info_get_subdevice_name(info);
    pdebug("name: %s\n      %s\n", name, sub_name);

    if (sub_name[0] == '\0') {
        pdebug("sub_name[0] == '\0");
    } else {
        sub = 0;
        for (;;) {
            pdebug("%c%c hw:%d,%d,%d %s", in ? 'I' : ' ', out ? 'O' : ' ', card,
                   device, sub, sub_name);
            if (++sub >= subs) {
                break;
            }

            in = is_input(ctl, card, device, sub);
            out = is_output(ctl, card, device, sub);
            snd_rawmidi_info_set_subdevice(info, sub);

            if (out) {
                snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);
                if ((status = snd_ctl_rawmidi_info(ctl, info)) < 0) {
                    error("could not get rawmidi info from hw:%d,%d,%d %s",
                          card, device, sub, sub_name);
                    break;
                }
            } else {
                snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);
                if ((status = snd_ctl_rawmidi_info(ctl, info)) < 0) {
                    error("could not get rawmidi info from hw:%d,%d,%d %s",
                          card, device, sub, sub_name);
                    break;
                }
            }

            sub_name = snd_rawmidi_info_get_subdevice_name(info);
        }
    }
}

// Returns true if given card/device/sub can output MIDI
static int is_output(snd_ctl_t* ctl, int card, int device, int sub) {
    snd_rawmidi_info_t* info;
    int status;

    snd_rawmidi_info_alloca(&info);
    snd_rawmidi_info_set_device(info, device);
    snd_rawmidi_info_set_subdevice(info, sub);
    snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);

    if ((status = snd_ctl_rawmidi_info(ctl, info)) < 0 && status != -ENXIO) {
        return status; // failed, return status code for parsing
    } else if (status == 0) {
        // we got a match, return success
        return 1;
    }

    // nope
    return 0;
}

// Returns true if specify card/device/sub can input MIDI
static int is_input(snd_ctl_t* ctl, int card, int device, int sub) {
    snd_rawmidi_info_t* info;
    int status;

    snd_rawmidi_info_alloca(&info);
    snd_rawmidi_info_set_device(info, device);
    snd_rawmidi_info_set_subdevice(info, sub);
    snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);

    if ((status = snd_ctl_rawmidi_info(ctl, info)) < 0 && status != -ENXIO) {
        return status; // failed, return status code for parsing
    } else if (status == 0) {
        // we got a match, return success
        return 1;
    }

    // nope
    return 0;
}

static char* char_port_types(unsigned index) {
    char* types = malloc(256 * sizeof(char));
    types[0] = '\0';

    if (contains_bit(index, SND_SEQ_PORT_TYPE_SPECIFIC)) {
        sprintf(types, "SPECIFIC");
    }

    if (contains_bit(index, SND_SEQ_PORT_TYPE_MIDI_GENERIC)) {
        sprintf(types, "%s, MIDI_GENERIC", types);
    }

    if (contains_bit(index, SND_SEQ_PORT_TYPE_MIDI_GM)) {
        sprintf(types, "%s, MMIDI_GM", types);
    }

    if (contains_bit(index, SND_SEQ_PORT_TYPE_MIDI_GS)) {
        sprintf(types, "%s, MIDI_GS", types);
    }

    if (contains_bit(index, SND_SEQ_PORT_TYPE_MIDI_XG)) {
        sprintf(types, "%s, MIDI_XG", types);
    }

    if (contains_bit(index, SND_SEQ_PORT_TYPE_MIDI_MT32)) {
        sprintf(types, "%s, MIDI_MT32", types);
    }

    if (contains_bit(index, SND_SEQ_PORT_TYPE_MIDI_GM2)) {
        sprintf(types, "%s, MIDI_GM2", types);
    }

    if (contains_bit(index, SND_SEQ_PORT_TYPE_SYNTH)) {
        sprintf(types, "%s, SYNTH", types);
    }

    if (contains_bit(index, SND_SEQ_PORT_TYPE_DIRECT_SAMPLE)) {
        sprintf(types, "%s, DIRECT_SAMPLE", types);
    }

    if (contains_bit(index, SND_SEQ_PORT_TYPE_SAMPLE)) {
        sprintf(types, "%s, SAMPLE", types);
    }

    if (contains_bit(index, SND_SEQ_PORT_TYPE_HARDWARE)) {
        sprintf(types, "%s, HARDWARE", types);
    }

    if (contains_bit(index, SND_SEQ_PORT_TYPE_SOFTWARE)) {
        sprintf(types, "%s, SOFTWARE", types);
    }

    if (contains_bit(index, SND_SEQ_PORT_TYPE_SYNTHESIZER)) {
        sprintf(types, "%s, SYNTHESIZER", types);
    }

    if (contains_bit(index, SND_SEQ_PORT_TYPE_PORT)) {
        sprintf(types, "%s, PORT", types);
    }

    if (contains_bit(index, SND_SEQ_PORT_TYPE_APPLICATION)) {
        sprintf(types, "%s, APPLICATION", types);
    }

    return types;
}

static char* char_port_capabilities(unsigned index) {
    char* caps = malloc(256 * sizeof(char));
    caps[0] = '\0';

    if (contains_bit(index, SND_SEQ_PORT_CAP_READ)) {
        sprintf(caps, "READ");
    }

    if (contains_bit(index, SND_SEQ_PORT_CAP_WRITE)) {
        sprintf(caps, "%s, WRITE", caps);
    }

    if (contains_bit(index, SND_SEQ_PORT_CAP_SYNC_READ)) {
        sprintf(caps, "%s, SYNC_READ", caps);
    }

    if (contains_bit(index, SND_SEQ_PORT_CAP_SYNC_WRITE)) {
        sprintf(caps, "%s, SYNC_WRITE", caps);
    }

    if (contains_bit(index, SND_SEQ_PORT_CAP_DUPLEX)) {
        sprintf(caps, "%s, DUPLEX", caps);
    }

    if (contains_bit(index, SND_SEQ_PORT_CAP_SUBS_READ)) {
        sprintf(caps, "%s, SUBS_READ", caps);
    }

    if (contains_bit(index, SND_SEQ_PORT_CAP_SUBS_READ)) {
        sprintf(caps, "%s, SUBS_WRITE", caps);
    }

    if (contains_bit(index, SND_SEQ_PORT_CAP_NO_EXPORT)) {
        sprintf(caps, "%s, NO_EXPORT", caps);
    }

    return caps;
}

static void init_sequencer(snd_seq_t** seq, char* name) {
    /* snd_seq_t* seq; */
    int status;

    if (status = snd_seq_open(*&seq, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
        error("Could not open sequencer: %s", snd_strerror(status));
        /* return NULL; */
    }

    if (name != NULL) {
        snd_seq_set_client_name(*seq, name);
    }

    /* return seq; */
}

static void create_ports(snd_seq_t* seq) {
    check_snd("create ports",
              snd_seq_create_simple_port(seq, "midi-mapper",
                                         SND_SEQ_PORT_CAP_WRITE |
                                             SND_SEQ_PORT_CAP_SUBS_WRITE,
                                         SND_SEQ_PORT_TYPE_MIDI_GENERIC |
                                             SND_SEQ_PORT_TYPE_APPLICATION));
}

static void connect_ports(snd_seq_t* seq, const ClientPort *cp) {
  int err = snd_seq_connect_from(seq, 0, cp->client, cp->port);
  check_snd("connecting ports", err);
}

static void check_snd(char* desc, int err) {
  if (err > 0) {
    error("Alsa Subsystem error: failed to %s", desc);
    exit(EXIT_FAILURE);
  }
}
#endif
