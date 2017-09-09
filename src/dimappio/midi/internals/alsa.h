#ifdef __linux__
#ifndef MIDI_INTERNAL_ALSA_H
#define MIDI_INTERNAL_ALSA_H

#include <alsa/asoundlib.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/poll.h>

#include "midi/device.h"
#include "midi/eventing.h"
#include "midi/internals/alsa_decoder.h"
#include "midi/mapping.h"
#include "midi/monitor.h"
#include "midi/nodes.h"
#include "midi/output.h"
#include "midi/parser.h"

#include "options.h"
#include "utils.h"

typedef snd_seq_t dm_midi_device;

dm_devices* dma_get_devices();
snd_seq_port_info_t* dma_get_port_info(dm_device* dev);
bool dma_client_exists(char* client);

void dma_monitor_device(dm_options* options);

void dma_send_midi_note(int client, int port, char* note, bool on, int ch,
                        int vel);

void dma_event_loop(dm_options* options, dm_midi_output* output,
                    void (*render_callback)(dm_options* options,
                                            dm_key_node* tail,
                                            dm_key_set* key_set),
                    void (*key_callback)(int key, int ch, int vel, int on));

int dma_init_sequencer(snd_seq_t** seq, char* name);
dm_midi_output* dma_midi_output_create(int client, int port);

void dma_receive_events_from(dm_midi_output* output, int client, int port);
void dma_send_events_to(dm_midi_output* output, int client, int port);
void dma_send_event(dm_midi_output* output, snd_seq_event_t* ev);
void dma_send_midi(dm_midi_output* output, int midi, bool on, int ch, int vel);

int dma_create_port(snd_seq_t* seq, char* name, unsigned caps, unsigned type);

#endif
#endif
