#ifndef MIDI_EVENTING_H
#define MIDI_EVENTING_H

#include "midi/driver.h"
#include "midi/nodes.h"
#include "midi/output.h"
#include "options.h"

int mm_event_process(mm_midi_output* output, mm_options* options,
                     mm_key_node* tail, mm_key_set** active_keyset, int midi,
                     int chan, int vel, int note_on);

#endif
