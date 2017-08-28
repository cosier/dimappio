#ifndef MIDI_OUTPUT_H
#define MIDI_OUTPUT_H

#ifdef __linux__
#include <alsa/asoundlib.h>

typedef struct mm_midi_output {
    snd_seq_t* dev;
    int id;

    int* out_ports;
    int out_count;

    int* in_ports;
    int in_count;

} mm_midi_output;

#endif
#endif
