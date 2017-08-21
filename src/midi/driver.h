#ifndef DRIVER_H
#define DRIVER_H

#include "midi/internals/alsa.h"
#include "midi/internals/coremidi.h"
#include "midi/internals/winmm.h"

#include "utils.h"

typedef struct Devices Devices;
typedef struct Device Device;

typedef struct MidiClients MidiClients;
typedef struct MidiClient MidiClient;

Device *MM_CreateVirtualDevice(char *name);
Devices *MM_GetDevices();
MidiClients *MM_GetClients();

void MM_MIDIReadProc(const MIDIPacketList *pktlist, void *refCon,
                     void *connRefCon);
void MM_MIDINotifyProc(const MIDINotification *message, void *refCon);

void MM_AttachListener(Device *dev,
                       void (*func)(const MIDIPacketList *message, void *refCon,
                                    void *connRefCon));

#endif
