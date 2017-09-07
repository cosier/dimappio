#ifndef MIDI_DEVICE_H
#define MIDI_DEVICE_H

#ifdef __linux__
#include <alsa/asoundlib.h>
typedef snd_seq_t dm_midi_device;
typedef snd_seq_event_t dm_midi_event;
#endif

typedef struct dm_device {
    char* name;
    int client;
    int port;
} dm_device;

typedef struct dm_devices {
    dm_device** store;
    int count;
} dm_devices;

typedef struct dm_midi_port {
    const char* name;
    int capability;
    int channels;
    int port_id;
    unsigned type;

} dm_midi_port;

void dm_devices_free(dm_devices* devices);

#endif
