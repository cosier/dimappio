#ifndef MIDI_MONITOR_H
#define MIDI_MONITOR_H

#include "midi/mapping.h"
#include "midi/nodes.h"
#include "options.h"

void dm_monitor_render(dm_options* options, dm_key_node* tail,
                       dm_key_set* key_set);

#endif
