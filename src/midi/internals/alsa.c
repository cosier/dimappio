#include "midi/internals/alsa.h"
#ifdef __linux__

Device *MMAlsa_CreateVirtualDevice(char *name) {
  /* printw("Creating virtual device: alsa\n"); */
  Device *dev;
  return dev;
}

Devices *MMAlsa_GetDevices() {
  Devices *devices = malloc(sizeof(Devices));
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

  snd_ctl_t *ctl;

  while (card >= 0) {
    rawmidi_devices_on_card(ctl, card);

    if ((status = snd_card_next(&card)) < 0) {
      error("No other cards available: %s", snd_strerror(status));
      break;
    }
  }

  return devices;
}

void MMAlsa_ClientDetails(MIDIClient *client) {
    char *type = "unknown";
    if (client->type == SND_SEQ_USER_CLIENT) {
        type = "user";

    } else if (client->type == SND_SEQ_KERNEL_CLIENT) {
        type = "kernel";
    }
    /* printf("[%d] %s (type: %s, card: %d)\n", */
    /*        client->client_id, */
    /*        client->name, */
    /*        type, */
    /*        client->card); */
    /* printf("  ports (%d)\n", client->num_ports); */

    MIDIClientPort *port;
    char * caps;
    char * types;
    unsigned cindex;
    for (int i = 0; i < client->num_ports; i++) {
        port = client->ports[i];
        types = char_port_types(port->type);
        printf("[%d:%d]: %s (ch: %d): %s\n",
               client->client_id,
               port->port_id,
               port->name,
               port->channels,
               types
            );


        caps = char_port_capabilities(port->capability);
        printf("    • %s\n", caps);

        printf("\n");
    }
}

MIDIClients *MMAlsa_GetClients() {
  MIDIClients *clients = malloc(sizeof(MIDIClients));

  // TODO: remove this hard limit and make dynamic via realloc
  clients->store = malloc(32 * sizeof(MIDIClient *));
  clients->count = 0;

  snd_seq_t *seq;

  if (snd_seq_open(&seq, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
    error("failed to open sequencer");
    return clients;
  }

  snd_seq_client_info_t *cinfo;
  snd_seq_port_info_t *pinfo;
  MIDIClient *client;
  MIDIClientPort *port;

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
    client->ports = malloc(16 * sizeof(MIDIClientPort *));

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

  return clients;
}

void rawmidi_devices_on_card(snd_ctl_t *ctl, int card) {
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

void rawmidi_subdevice_info(snd_ctl_t *ctl, int card, int device) {
  pdebug("reading subdevice info");
  snd_rawmidi_info_t *info;

  const char *name;
  const char *sub_name;

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
    error("cannot get rawmidi sub output information %d:%d %s", card, device,
          snd_strerror(status));
    return;
  } else if (status) {
    // is_output determined the sub is an output
    out = 1;
  }

  // if is_output did not determine outputness, try is_input
  if (status) {
    if ((status = is_input(ctl, card, device, sub)) < 0) {
      // is_output failed with a status code
      error("cannot get rawmidi sub input information %d:%d %s", card, device,
            snd_strerror(status));
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
          error("could not get rawmidi info from hw:%d,%d,%d %s", card, device,
                sub, sub_name);
          break;
        }
      } else {
        snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);
        if ((status = snd_ctl_rawmidi_info(ctl, info)) < 0) {
          error("could not get rawmidi info from hw:%d,%d,%d %s", card, device,
                sub, sub_name);
          break;
        }
      }

      sub_name = snd_rawmidi_info_get_subdevice_name(info);
    }
  }
}

// Returns true if given card/device/sub can output MIDI
int is_output(snd_ctl_t *ctl, int card, int device, int sub) {
  snd_rawmidi_info_t *info;
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
int is_input(snd_ctl_t *ctl, int card, int device, int sub) {
  snd_rawmidi_info_t *info;
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

char* char_port_types(unsigned index) {
    char *types = malloc(256 * sizeof(char));
    types[0] = '\0';

    if (contains_bit(index,  SND_SEQ_PORT_TYPE_SPECIFIC)) {
        sprintf(types, "SPECIFIC");
    }

    if (contains_bit(index,  SND_SEQ_PORT_TYPE_MIDI_GENERIC)) {
        sprintf(types, "%s, MIDI_GENERIC", types);
    }

    if (contains_bit(index,  SND_SEQ_PORT_TYPE_MIDI_GM)) {
        sprintf(types, "%s, MMIDI_GM", types);
    }

    if (contains_bit(index,  SND_SEQ_PORT_TYPE_MIDI_GS)) {
        sprintf(types, "%s, MIDI_GS", types);
    }

    if (contains_bit(index,  SND_SEQ_PORT_TYPE_MIDI_XG)) {
        sprintf(types, "%s, MIDI_XG", types);
    }

    if (contains_bit(index,  SND_SEQ_PORT_TYPE_MIDI_MT32)) {
        sprintf(types, "%s, MIDI_MT32", types);
    }

    if (contains_bit(index,  SND_SEQ_PORT_TYPE_MIDI_GM2)) {
        sprintf(types, "%s, MIDI_GM2", types);
    }

    if (contains_bit(index,  SND_SEQ_PORT_TYPE_SYNTH)) {
        sprintf(types, "%s, SYNTH", types);
    }

    if (contains_bit(index,  SND_SEQ_PORT_TYPE_DIRECT_SAMPLE)) {
        sprintf(types, "%s, DIRECT_SAMPLE", types);
    }

    if (contains_bit(index,  SND_SEQ_PORT_TYPE_SAMPLE)) {
        sprintf(types, "%s, SAMPLE", types);
    }

    if (contains_bit(index,  SND_SEQ_PORT_TYPE_HARDWARE)) {
        sprintf(types, "%s, HARDWARE", types);
    }

    if (contains_bit(index,  SND_SEQ_PORT_TYPE_SOFTWARE)) {
        sprintf(types, "%s, SOFTWARE", types);
    }

    if (contains_bit(index,  SND_SEQ_PORT_TYPE_SYNTHESIZER)) {
        sprintf(types, "%s, SYNTHESIZER", types);
    }

    if (contains_bit(index,  SND_SEQ_PORT_TYPE_PORT)) {
        sprintf(types, "%s, PORT", types);
    }

    if (contains_bit(index,  SND_SEQ_PORT_TYPE_APPLICATION)) {
        sprintf(types, "%s, APPLICATION", types);
    }

    return types;
}

char* char_port_capabilities(unsigned index) {
    char *caps = malloc(256 * sizeof(char));
    caps[0] = '\0';

    if (contains_bit(index,  SND_SEQ_PORT_CAP_READ)) {
        sprintf(caps, "READ");
    }

    if (contains_bit(index,  SND_SEQ_PORT_CAP_WRITE)) {
        sprintf(caps, "%s, WRITE", caps);
    }

    if (contains_bit(index,  SND_SEQ_PORT_CAP_SYNC_READ)) {
        sprintf(caps, "%s, SYNC_READ", caps);
    }

    if (contains_bit(index,  SND_SEQ_PORT_CAP_SYNC_WRITE)) {
        sprintf(caps, "%s, SYNC_WRITE", caps);
    }

    if (contains_bit(index,  SND_SEQ_PORT_CAP_DUPLEX)) {
        sprintf(caps, "%s, DUPLEX", caps);
    }

    if (contains_bit(index,  SND_SEQ_PORT_CAP_SUBS_READ)) {
        sprintf(caps, "%s, SUBS_READ", caps);
    }

    if (contains_bit(index,  SND_SEQ_PORT_CAP_SUBS_READ)) {
        sprintf(caps, "%s, SUBS_WRITE", caps);
    }

    if (contains_bit(index,  SND_SEQ_PORT_CAP_NO_EXPORT)) {
        sprintf(caps, "%s, NO_EXPORT", caps);
    }

    return caps;
}

#endif
