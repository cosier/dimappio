#ifdef __linux__
#ifndef INTERNAL_ALSA_DECODER_H
#define INTERNAL_ALSA_DECODER_H

#include <alsa/asoundlib.h>

#include "midi/internals/alsa.h"
#include "utils.h"

typedef snd_seq_event_t MIDIEvent;

char* MMA_char_port_types(unsigned index);
char* MMA_char_port_capabilities(unsigned index);
char* MMA_event_decoder(MIDIEvent* ev);

#endif
#endif
