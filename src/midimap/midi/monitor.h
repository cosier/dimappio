#ifndef MIDI_MONITOR_H
#define MIDI_MONITOR_H

#include "midi/mapping.h"
#include "midi/nodes.h"
#include "options.h"

void mm_monitor_render(mm_options* options, mm_key_node* tail,
                       mm_key_set* key_set);

#endif
