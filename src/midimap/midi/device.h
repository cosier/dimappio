#ifndef MIDI_DEVICE_H
#define MIDI_DEVICE_H

#ifdef __linux__
#include <alsa/asoundlib.h>
typedef snd_seq_t mm_midi_device;
#endif

typedef struct mm_device {
    const char* name;
    int client;
    int port;
} mm_device;

typedef struct mm_devices {
    mm_device** store;
    int count;
} mm_devices;

typedef struct mm_midi_port {
    const char* name;
    int capability;
    int channels;
    int port_id;
    unsigned type;

} mm_midi_port;

void mm_devices_free(mm_devices* devices);

#endif
