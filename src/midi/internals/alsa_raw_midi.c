#ifdef __linux__
#include "midi/internals/alsa_raw_midi.h"

void MMA_rawmidi_devices_on_card(snd_ctl_t* ctl, int card) {
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
                MMA_rawmidi_subdevice_info(ctl, card, device);

            } else {
                /* printf("unable to open rawmidi for device: %s %d\n", */
                /*        name, device); */
            }
        } while (device >= 0);
    }
}

void MMA_rawmidi_subdevice_info(snd_ctl_t* ctl, int card, int device) {
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

#endif
