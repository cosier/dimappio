#ifndef MIDI_INTERNAL_ALSA_H
#define MIDI_INTERNAL_ALSA_H
#ifdef __linux__

#include <alsa/asoundlib.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/poll.h>

#include "midi/parser.h"
#include "utils.h"

typedef int32_t MIDIObjectRef;
typedef MIDIObjectRef MIDIEndpointRef;

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

Devices* MMAlsa_GetDevices();
Device* MMAlsa_CreateVirtualDevice(char* name);

MIDIClients* MMAlsa_GetClients(snd_seq_t* seq);

void MMAlsa_ClientDetails(MIDIClient* client);
bool MMAlsa_ClientExists(char* client);
snd_seq_port_info_t* MMAlsa_GetClientPortInfo(snd_seq_t* seq,
                                              char* client_with_port);

void MMAlsa_MonitorDevice(char* client_with_port);

static void rawmidi_devices_on_card(snd_ctl_t* ctl, int card);
static void rawmidi_subdevice_info(snd_ctl_t* ctl, int card, int device);

static char* char_port_types(unsigned index);
static char* char_port_capabilities(unsigned index);

static int is_input(snd_ctl_t* ctl, int card, int device, int sub);
static int is_output(snd_ctl_t* ctl, int card, int device, int sub);

static snd_seq_t* init_sequencer(char* name);
#endif
#endif
