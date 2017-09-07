#ifndef MIDI_EVENTING_H
#define MIDI_EVENTING_H

#include "midi/driver.h"
#include "midi/nodes.h"
#include "midi/output.h"
#include "options.h"

int dm_event_process(void (*key_callback)(int key, int ch, int vel, int on),
                     dm_midi_output* output, dm_options* options,
                     dm_key_node* tail, dm_key_set** active_keyset, int midi,
                     int chan, int vel, int note_on);

#endif
