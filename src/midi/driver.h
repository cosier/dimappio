#ifndef DRIVER_H
#define DRIVER_H

#include "midi/internals/alsa.h"
#include "midi/internals/coremidi.h"
#include "midi/internals/winmm.h"

#include "utils.h"
extern bool MM_driver_debug_mode;

typedef struct Devices Devices;
typedef struct Device Device;

typedef struct MIDIClient MIDIClient;
typedef struct MIDIClient MIDIClient;

Device *MM_CreateVirtualDevice(char *name);
Devices *MM_GetDevices();
MIDIClients *MM_GetClients();

void MM_MIDIReadProc(const MIDIPacketList *pktlist, void *refCon,
                     void *connRefCon);
void MM_MIDINotifyProc(const MIDINotification *message, void *refCon);

void MM_AttachListener(Device *dev,
                       void (*func)(const MIDIPacketList *message, void *refCon,
                                    void *connRefCon));

void MM_ListClients();
void MM_ClientDetails(MIDIClient *client);
bool MM_ClientExists(char *client);
void MM_SendMidiNote(char *client, char *port, char *note);

void MM_DriverDebug();

#endif
