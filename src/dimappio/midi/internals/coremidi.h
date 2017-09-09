#ifndef INTERNAL_COREMIDI_H
#define INTERNAL_COREMIDI_H
#ifdef __APPLE__
#include <CoreAudio/HostTime.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#include <CoreMIDI/CoreMIDI.h>
#include <CoreMIDI/MIDIServices.h>

#include <AudioToolbox/AudioToolbox.h>

#include "dimappio/driver.h"
#include <ubelt/utils.h>

typedef struct Device {
    char* name;
    MIDIEndpointRef endpoint;
} Device;

typedef struct Devices {
    Device** store;
    int count;
} Devices;

Device* dmc_create_virtual_device();
Devices* dmc_get_devices();

void dmc_midi_read_proc(const MIDIPacketList* pktlist, void* refCon,
                        void* connRefCon);
void dmc_midi_notify_proc(const MIDINotification* message, void* refCon);
void dmc_attach_listener(Device dev,
                         void (*func)(const MIDINotification* message,
                                      void* refCon));

#endif
#endif
