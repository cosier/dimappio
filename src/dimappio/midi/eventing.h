#ifndef MIDI_EVENTING_H
#define MIDI_EVENTING_H

#include "dimappio/midi/driver.h"
#include "dimappio/midi/nodes.h"
#include "dimappio/midi/output.h"
#include "dimappio/options.h"

int dm_event_process(void (*key_callback)(int key, int ch, int vel, int on),
                     dm_midi_output* output, dm_options* options,
                     dm_key_node* tail, dm_key_set** active_keyset, int midi,
                     int chan, int vel, int note_on);

#endif
