#include "midi/internals/alsa_decoder.h"

char* mma_event_decoder(MIDIEvent* ev) {
    char* buf = malloc(sizeof(char) * 128);

    switch (ev->type) {
    case SND_SEQ_EVENT_NOTEON:
        if (ev->data.note.velocity)
            sprintf(buf, "Note on                %2d, note %d, velocity %d",
                    ev->data.note.channel, ev->data.note.note,
                    ev->data.note.velocity);
        else
            sprintf(buf, "Note off               %2d, note %d",
                    ev->data.note.channel, ev->data.note.note);
        break;
    case SND_SEQ_EVENT_NOTEOFF:
        sprintf(buf, "Note off               %2d, note %d, velocity %d",
                ev->data.note.channel, ev->data.note.note,
                ev->data.note.velocity);
        break;
    case SND_SEQ_EVENT_KEYPRESS:
        sprintf(buf, "Polyphonic aftertouch  %2d, note %d, value %d",
                ev->data.note.channel, ev->data.note.note,
                ev->data.note.velocity);
        break;
    case SND_SEQ_EVENT_CONTROLLER:
        sprintf(buf, "Control change         %2d, controller %d, value %d",
                ev->data.control.channel, ev->data.control.param,
                ev->data.control.value);
        break;
    case SND_SEQ_EVENT_PGMCHANGE:
        sprintf(buf, "Program change         %2d, program %d",
                ev->data.control.channel, ev->data.control.value);
        break;
    case SND_SEQ_EVENT_CHANPRESS:
        sprintf(buf, "Channel aftertouch     %2d, value %d",
                ev->data.control.channel, ev->data.control.value);
        break;
    case SND_SEQ_EVENT_PITCHBEND:
        sprintf(buf, "Pitch bend             %2d, value %d",
                ev->data.control.channel, ev->data.control.value);
        break;
    case SND_SEQ_EVENT_CONTROL14:
        sprintf(buf, "Control change         %2d, controller %d, value %5d",
                ev->data.control.channel, ev->data.control.param,
                ev->data.control.value);
        break;
    case SND_SEQ_EVENT_NONREGPARAM:
        sprintf(buf, "Non-reg. parameter     %2d, parameter %d, value %d",
                ev->data.control.channel, ev->data.control.param,
                ev->data.control.value);
        break;
    case SND_SEQ_EVENT_REGPARAM:
        sprintf(buf, "Reg. parameter         %2d, parameter %d, value %d",
                ev->data.control.channel, ev->data.control.param,
                ev->data.control.value);
        break;
    case SND_SEQ_EVENT_SONGPOS:
        sprintf(buf, "Song position pointer      value %d",
                ev->data.control.value);
        break;
    case SND_SEQ_EVENT_SONGSEL:
        sprintf(buf, "Song select                value %d",
                ev->data.control.value);
        break;
    case SND_SEQ_EVENT_QFRAME:
        sprintf(buf, "MTC quarter frame          %02xh",
                ev->data.control.value);
        break;
    case SND_SEQ_EVENT_TIMESIGN:
        // XXX how is this encoded?
        sprintf(buf, "SMF time signature         (%#010x)",
                ev->data.control.value);
        break;
    case SND_SEQ_EVENT_KEYSIGN:
        // XXX how is this encoded?
        sprintf(buf, "SMF key signature          (%#010x)",
                ev->data.control.value);
        break;
    case SND_SEQ_EVENT_START:
        if (ev->source.client == SND_SEQ_CLIENT_SYSTEM &&
            ev->source.port == SND_SEQ_PORT_SYSTEM_TIMER)
            sprintf(buf, "Queue start                queue %d",
                    ev->data.queue.queue);
        else
            sprintf(buf, "Start");
        break;
    case SND_SEQ_EVENT_CONTINUE:
        if (ev->source.client == SND_SEQ_CLIENT_SYSTEM &&
            ev->source.port == SND_SEQ_PORT_SYSTEM_TIMER)
            sprintf(buf, "Queue continue             queue %d",
                    ev->data.queue.queue);
        else
            sprintf(buf, "Continue");
        break;
    case SND_SEQ_EVENT_STOP:
        if (ev->source.client == SND_SEQ_CLIENT_SYSTEM &&
            ev->source.port == SND_SEQ_PORT_SYSTEM_TIMER)
            sprintf(buf, "Queue stop                 queue %d",
                    ev->data.queue.queue);
        else
            sprintf(buf, "Stop");
        break;
    case SND_SEQ_EVENT_SETPOS_TICK:
        sprintf(buf, "Set tick queue pos.        queue %d",
                ev->data.queue.queue);
        break;
    case SND_SEQ_EVENT_SETPOS_TIME:
        sprintf(buf, "Set rt queue pos.          queue %d",
                ev->data.queue.queue);
        break;
    case SND_SEQ_EVENT_TEMPO:
        sprintf(buf, "Set queue tempo            queue %d",
                ev->data.queue.queue);
        break;
    case SND_SEQ_EVENT_CLOCK:
        sprintf(buf, "Clock");
        break;
    case SND_SEQ_EVENT_TICK:
        sprintf(buf, "Tick");
        break;
    case SND_SEQ_EVENT_QUEUE_SKEW:
        sprintf(buf, "Queue timer skew           queue %d",
                ev->data.queue.queue);
        break;
    case SND_SEQ_EVENT_TUNE_REQUEST:
        sprintf(buf, "Tune request");
        break;
    case SND_SEQ_EVENT_RESET:
        sprintf(buf, "Reset");
        break;
    case SND_SEQ_EVENT_SENSING:
        sprintf(buf, "Active Sensing");
        break;
    case SND_SEQ_EVENT_CLIENT_START:
        sprintf(buf, "Client start               client %d",
                ev->data.addr.client);
        break;
    case SND_SEQ_EVENT_CLIENT_EXIT:
        sprintf(buf, "Client exit                client %d",
                ev->data.addr.client);
        break;
    case SND_SEQ_EVENT_CLIENT_CHANGE:
        sprintf(buf, "Client changed             client %d",
                ev->data.addr.client);
        break;
    case SND_SEQ_EVENT_PORT_START:
        sprintf(buf, "Port start                 %d:%d", ev->data.addr.client,
                ev->data.addr.port);
        break;
    case SND_SEQ_EVENT_PORT_EXIT:
        sprintf(buf, "Port exit                  %d:%d", ev->data.addr.client,
                ev->data.addr.port);
        break;
    case SND_SEQ_EVENT_PORT_CHANGE:
        sprintf(buf, "Port changed               %d:%d", ev->data.addr.client,
                ev->data.addr.port);
        break;
    case SND_SEQ_EVENT_PORT_SUBSCRIBED:
        sprintf(buf, "Port subscribed            %d:%d -> %d:%d",
                ev->data.connect.sender.client, ev->data.connect.sender.port,
                ev->data.connect.dest.client, ev->data.connect.dest.port);
        break;
    case SND_SEQ_EVENT_PORT_UNSUBSCRIBED:
        sprintf(buf, "Port unsubscribed          %d:%d -> %d:%d",
                ev->data.connect.sender.client, ev->data.connect.sender.port,
                ev->data.connect.dest.client, ev->data.connect.dest.port);
        break;
    case SND_SEQ_EVENT_SYSEX: {
        unsigned int i;
        sprintf(buf, "System exclusive          ");
        for (i = 0; i < ev->data.ext.len; ++i)
            sprintf(buf, " %02X", ((unsigned char*)ev->data.ext.ptr)[i]);
        /* sprintf(buf, "\n"); */
    } break;
    default:
        sprintf(buf, "Event type %d", ev->type);
    }
    return buf;
}

char* mma_char_port_types(unsigned index) {
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

char* mma_char_port_capabilities(unsigned index) {
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
