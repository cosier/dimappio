#ifndef DRIVER_H
#define DRIVER_H

#include <ctype.h>

#include "dimappio/midi/device.h"
#include "dimappio/midi/internals/alsa.h"
#include "dimappio/midi/internals/coremidi.h"
#include "dimappio/midi/internals/winmm.h"
#include "dimappio/midi/mapping.h"
#include "dimappio/midi/output.h"
#include "dimappio/options.h"

#include <ubelt/utils.h>

extern bool dm_driver_debug_mode;

dm_devices* dm_get_devices();
dm_device* dm_get_midi_through();

void dm_monitor_client(dm_options* options);
void dm_event_loop(dm_options* options, dm_midi_output* output,
                   void (*render_callback)(dm_options* options,
                                           dm_key_node* tail,
                                           dm_key_set* key_set),
                   void (*key_callback)(int key, int ch, int vel, int on));

void dm_list_clients();
bool dm_client_exists(char* client);
void dm_send_midi_to_client(int client, int port, char* note, bool on, int ch,
                            int vel);

void dm_send_midi(dm_midi_output* output, int midi, bool on, int ch, int vel);
void dm_send_event(dm_midi_output* output, dm_midi_event* ev);

void dm_driver_init(dm_midi_device**, char* name);

void dm_receive_events_from(dm_midi_output* output, int client, int port);
void dm_send_events_to(dm_midi_output* output, int client, int port);

void dm_driver_debug();

#endif
