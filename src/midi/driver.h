#ifndef DRIVER_H
#define DRIVER_H

#include "midi/internals/alsa.h"
#include "midi/internals/coremidi.h"
#include "midi/internals/winmm.h"
#include "midi/mapping.h"

#include "utils.h"
extern bool mm_driver_debug_mode;

typedef struct Devices Devices;
typedef struct Device Device;

typedef struct MIDIClient MIDIClient;
typedef struct MIDIClient MIDIClient;

Device* mm_create_virtual_device(char* name);
Devices* mm_get_devices();
MIDIClients* mm_get_clients();

void mm_monitor_client(char* client_with_port, mm_mapping *mappings);

void mm_attach_listener(Device* dev,
                       void (*func)(const MIDIPacketList* message, void* refCon,
                                    void* connRefCon));

void mm_list_clients();
void mm_client_details(MIDIClient* client);
bool mm_client_exists(char* client);
void mm_send_midi_note(char* client, char* port, char* note);

void mm_driver_debug();

#endif
