#ifndef DRIVER_H
#define DRIVER_H

#include "midi/internals/alsa.h"
#include "midi/internals/coremidi.h"
#include "midi/internals/winmm.h"

#include "utils.h"

typedef struct Devices Devices;

extern int MM_CreateVirtualDevice(char *name);
extern Devices *MM_GetDevices();

void MM_MIDIReadProc(const MIDIPacketList *pktlist, void *refCon,
                     void *connRefCon);
void MM_MIDINotifyProc(const MIDINotification *message, void *refCon);

void MM_AttachListener(Device *dev,
                       void (*func)(const MIDIPacketList *message, void *refCon,
                                    void *connRefCon));

#endif
