#ifndef MIDI_MONITOR_H
#define MIDI_MONITOR_H

#include "dimappio/midi/mapping.h"
#include "dimappio/midi/nodes.h"
#include "dimappio/options.h"
#include <ubelt/colors.h>

void dm_monitor_render(dm_options* options, dm_key_node* tail,
                       dm_key_set* key_set);

#endif
