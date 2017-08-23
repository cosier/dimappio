#include "midi/internals/alsa.h"
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
        MMA_rawmidi_devices_on_card(ctl, card);

        if ((status = snd_card_next(&card)) < 0) {
            error("No other cards available: %s", snd_strerror(status));
            break;
        }
    }

    return devices;
}

void MMAlsa_MonitorDevice(char* client_with_port) {
    snd_seq_t* seq;

    int seq_id = init_sequencer(&seq, "midimap-monitor");
    snd_seq_nonblock(seq, 1);

    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);

    const MIDIPortInfo* pinfo = MMAlsa_GetPortInfo(client_with_port);
    const MIDIAddr* addr = snd_seq_port_info_get_addr(pinfo);
    const char* pname = snd_seq_port_info_get_name(pinfo);
    const ClientPort* cp = parseStringToClientPort(client_with_port);

    int seq_port = create_port(seq);
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
                process_event(event, seq_id, seq_port);
            }

        } while (err > 0);

        if (stop) {
            break;
        }
    }

    snd_seq_close(seq);
}

static void process_event(MIDIEvent* ev, int seq_id, int seq_port) {
    char note[] = "NOTE";

    printf("[%3d:%2d ] ", ev->source.client, ev->source.port);
    char *edesc = MMA_event_decoder(ev);
    printf("%s", edesc);
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
        types = MMA_char_port_types(port->type);
        printf("[%d:%d] %s : %s (ch: %d): [%s: %s]\n", client->client_id,
               port->port_id, client->name, port->name, port->channels, type,
               types);

        caps = MMA_char_port_capabilities(port->capability);
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

static int init_sequencer(snd_seq_t** seq, char* name) {
    /* snd_seq_t* seq; */
    int status;

    if (status = snd_seq_open(*&seq, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
        error("Could not open sequencer: %s", snd_strerror(status));
        /* return NULL; */
    }

    if (name != NULL) {
        snd_seq_set_client_name(*seq, name);
    }

    return snd_seq_client_id(*seq);
}

static int create_port(snd_seq_t* seq) {
    int port_result = snd_seq_create_simple_port(
        seq, "midi-mapper",
        SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_WRITE |
            SND_SEQ_PORT_CAP_SUBS_READ | SND_SEQ_PORT_CAP_SUBS_WRITE,
        SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION);
    return port_result;
}

static void connect_ports(snd_seq_t* seq, const ClientPort* cp) {
    int err = snd_seq_connect_from(seq, 0, cp->client, cp->port);
    check_snd("connecting ports", err);
}

static void check_snd(char* desc, int err) {
    if (err < 0) {
        error("Alsa Subsystem error: failed to %s because %s", desc,
              snd_strerror(err));
        exit(EXIT_FAILURE);
    }
}
#endif
