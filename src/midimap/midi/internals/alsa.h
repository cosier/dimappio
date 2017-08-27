#ifdef __linux__
#ifndef MIDI_INTERNAL_ALSA_H
#define MIDI_INTERNAL_ALSA_H

#include <alsa/asoundlib.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/poll.h>

#include "midi/internals/alsa_decoder.h"
#include "midi/internals/alsa_raw_midi.h"
#include "midi/mapping.h"
#include "midi/nodes.h"
#include "midi/parser.h"
#include "utils.h"

typedef int32_t MIDIObjectRef;
typedef MIDIObjectRef MIDIEndpointRef;

typedef snd_seq_port_info_t MIDIPortInfo;
typedef snd_seq_client_info_t MIDIClientInfo;
typedef snd_seq_addr_t MIDIAddr;
typedef snd_seq_event_t MIDIEvent;

typedef struct MIDIClientPort {
    const char* name;
    int capability;
    int channels;
    int port_id;
    uint type;
    const snd_seq_addr_t* addr;

} MIDIClientPort;

typedef struct MIDIClient {
    const char* name;
    snd_seq_client_type_t type;
    int client_id;
    int card;
    int pid;
    int num_ports;
    MIDIClientPort** ports;
} MIDIClient;

typedef struct MIDIClients {
    int8_t count;
    MIDIClient** store;
} MIDIClients;

typedef struct Device {
    char* name;
    MIDIEndpointRef endpoint;
} Device;

typedef struct Devices {
    Device** store;
    int count;
} Devices;

typedef int64_t MIDITimestamp;

typedef struct MidiPacket {
    MIDITimestamp timestamp;
    int16_t length;
    int32_t* data[];
} MidiPacket;

typedef struct MIDINotification {
    int32_t messageID;
    int32_t messageSize;
} MIDINotification;

typedef struct MIDIPacketList {
    int32_t numPackets;
    MidiPacket packet;
} MIDIPacketList;

Devices* mma_get_devices();
Device* mma_create_virtual_device(char* name);

MIDIClients* mma_get_clients(snd_seq_t* seq);
MIDIPortInfo* mma_get_port_info(char* client_with_port);

void mma_client_details(MIDIClient* client);
bool mma_client_exists(char* client);
void mma_monitor_device(char* client_with_port, mm_mapping* mappings);

void mma_send_midi_note(int client, int port, char* note);

static int init_sequencer(snd_seq_t** seq, char* name);
static void process_event(snd_seq_event_t* event, snd_seq_t* seq, int seq_port,
                          mm_key_node** tail);

static void send_event(snd_seq_t* seq, int port, snd_seq_event_t* event);
static void send_midi(snd_seq_t* seq, int port, int midi, bool on);
static void send_event_to_client_port(snd_seq_t* seq, int client, int port,
                                     snd_seq_event_t* ev);

static int create_port(snd_seq_t* seq);
static void connect_ports(snd_seq_t* seq, const ClientPort* cp);
static void check_snd(char* desc, int err);

static void trigger_mapping(snd_seq_t* seq, int seq_port,
                            snd_seq_event_t* event, int dsts_count, int* dsts);
static void release_mapping(snd_seq_t* seq, int seq_port,
                            snd_seq_event_t* event, int dsts_count, int* dsts);
#endif
#endif
