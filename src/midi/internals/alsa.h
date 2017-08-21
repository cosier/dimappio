#ifndef MIDI_INTERNAL_ALSA_H
#define MIDI_INTERNAL_ALSA_H
#ifdef __linux__

#include <stdint.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#include "utils.h"

typedef int32_t MIDIObjectRef;
typedef MIDIObjectRef MIDIEndpointRef;

typedef struct MIDIClientPort {
  const char *name;
  int capability;
  int channels;
  int port_id;
  uint type;
  const snd_seq_addr_t *addr;

} MIDIClientPort;

typedef struct MIDIClient {
  const char *name;
  snd_seq_client_type_t type;
  int client_id;
  int card;
  int pid;
  int num_ports;
  MIDIClientPort **ports;
} MIDIClient;


typedef struct MIDIClients {
  int8_t count;
  MIDIClient **store;
} MIDIClients;


typedef struct Device {
  char *name;
  MIDIEndpointRef endpoint;
} Device;

typedef struct Devices {
  Device **store;
  int count;
} Devices;

typedef int64_t MIDITimestamp;

typedef struct MidiPacket {
  MIDITimestamp timestamp;
  int16_t length;
  int32_t *data[];
} MidiPacket;

typedef struct MIDINotification {
  int32_t messageID;
  int32_t messageSize;
} MIDINotification;

typedef struct MIDIPacketList {
  int32_t numPackets;
  MidiPacket packet;
} MIDIPacketList;


Devices *MMAlsa_GetDevices();
MIDIClients *MMAlsa_GetClients();

Device *MMAlsa_CreateVirtualDevice(char *name);

#endif
#endif
