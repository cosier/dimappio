#ifndef INTERNAL_COREMIDI_H
#define INTERNAL_COREMIDI_H
#ifdef __APPLE__
#include <CoreAudio/HostTime.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#include <CoreMIDI/CoreMIDI.h>
#include <CoreMIDI/MIDIServices.h>

#include <AudioToolbox/AudioToolbox.h>

#include "midi/driver.h"
#include "utils.h"

typedef struct Device {
    char* name;
    MIDIEndpointRef endpoint;
} Device;

typedef struct Devices {
    Device** store;
    int count;
} Devices;

Device* mmc_create_virtual_device();
Devices* mmc_get_devices();

void mmc_midi_read_proc(const MIDIPacketList* pktlist, void* refCon,
                        void* connRefCon);
void mmc_midi_notify_proc(const MIDINotification* message, void* refCon);
void mmc_attach_listener(Device dev,
                         void (*func)(const MIDINotification* message,
                                      void* refCon));

#endif
#endif
