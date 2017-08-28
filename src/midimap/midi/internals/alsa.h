#ifdef __linux__
#ifndef MIDI_INTERNAL_ALSA_H
#define MIDI_INTERNAL_ALSA_H

#include <alsa/asoundlib.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/poll.h>

#include "midi/device.h"
#include "midi/internals/alsa_decoder.h"
#include "midi/internals/alsa_raw_midi.h"
#include "midi/mapping.h"
#include "midi/nodes.h"
#include "midi/output.h"
#include "midi/parser.h"
#include "utils.h"

typedef snd_seq_t mm_midi_device;

mm_devices* mma_get_devices();
snd_seq_port_info_t* mma_get_port_info(mm_device* dev);
bool mma_client_exists(char* client);

void mma_monitor_device(char* source, char* target, mm_mapping* mappings);

void mma_send_midi_note(int client, int port, char* note, bool on, int ch,
                        int vel);

void mma_event_loop(mm_mapping* mapping, mm_midi_output* output);

int mma_init_sequencer(snd_seq_t** seq, char* name);
mm_midi_output* mma_midi_output_create(int client, int port);

void mma_receive_events_from(mm_midi_output* output, int client, int port);
void mma_send_events_to(mm_midi_output* output, int client, int port);

int mma_create_port(snd_seq_t* seq, char* name, unsigned caps, unsigned type);

static void update_node_list(snd_seq_event_t* event, mm_key_node** tail);

static void send_event(mm_midi_output* output, snd_seq_event_t* event);
static void send_midi(mm_midi_output* output, int midi, bool on, int ch,
                      int vel);

static void check_snd(char* desc, int err);

static void trigger_mapping(mm_midi_output* output, snd_seq_event_t* event,
                            mm_key_set* dsts_set);

static void release_mapping(mm_midi_output* output, snd_seq_event_t* event,
                            mm_key_set* dsts_set);
#endif
#endif
