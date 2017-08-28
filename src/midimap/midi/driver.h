#ifndef DRIVER_H
#define DRIVER_H

#include <ctype.h>

#include "midi/device.h"
#include "midi/internals/alsa.h"
#include "midi/internals/coremidi.h"
#include "midi/internals/winmm.h"
#include "midi/mapping.h"
#include "midi/output.h"

#include "utils.h"

extern bool mm_driver_debug_mode;

mm_devices* mm_get_devices();
mm_device* mm_get_midi_through();

void mm_monitor_client(char* source, char* target, mm_mapping* mappings);

void mm_list_clients();
bool mm_client_exists(char* client);
void mm_send_midi_note(int client, int port, char* note, bool on, int ch,
                       int vel);

void mm_driver_init(mm_midi_device**, char* name);

void mm_receive_events_from(mm_midi_output* output, int client, int port);
void mm_send_events_to(mm_midi_output* output, int client, int port);

void mm_driver_debug();

#endif
