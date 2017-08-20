#ifndef MIDI_INTERNAL_ALSA_H
#define MIDI_INTERNAL_ALSA_H
#ifdef __linux__

#include <stdint.h>
typedef struct Devices Devices;

typedef int32_t MIDIObjectRef;
typedef MIDIObjectRef MIDIEndpointRef;

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

#endif
#endif
