#ifdef __linux__
#ifndef INTERNAL_ALSA_RAW_MIDI_H
#define INTERNAL_ALSA_RAW_MIDI_H
#include "utils.h"
#include <alsa/asoundlib.h>

void mma_rawmidi_devices_on_card(snd_ctl_t* ctl, int card);
void mma_rawmidi_subdevice_info(snd_ctl_t* ctl, int card, int device);

static int is_input(snd_ctl_t* ctl, int card, int device, int sub);
static int is_output(snd_ctl_t* ctl, int card, int device, int sub);

#endif
#endif
