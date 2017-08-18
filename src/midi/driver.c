#include "driver.h"

typedef struct Connection {
  int id;
} Connection;

typedef struct MIDIPlayer {
  AUGraph graph;
  AudioUnit instrumentUnit;
} MIDIPlayer;

void MyMIDIReadProc(const MIDIPacketList *pktlist,
    void *refCon,
    void *connRefCon) {

  Connection *c = (Connection*) refCon;
  printf("received midi: %d", c->id);
}

void MyMIDINotifyProc(const MIDINotification *message, void *refCon) {
  printf("MIDI Notify -> messageID=%d", message->messageID);
  Connection *c = (Connection*) refCon;
}

int CreateVirtualDevice() {
  CFStringRef name  = CFSTR("midimapper.virtual");

  MIDIClientRef vClient;
  MIDIEndpointRef vEndpoint;

  MIDIPortRef *output = NULL;
  MIDIPortRef *input = NULL;

  Connection con;
  con.id = 1;

  MIDIClientCreate(CFSTR("midimapper.client"),
                              MyMIDINotifyProc,
                              &con,
                              &vClient);

  MIDIOutputPortCreate(vClient, CFSTR("midimapper.output"), output);
  /* MIDIInputPortCreate(vClient, CFSTR("midimapper.input"), MyMIDIReadProc, &con, output); */

  MIDIDestinationCreate(vClient, name, MyMIDIReadProc, &con, &vEndpoint);
  MIDISourceCreate(vClient, name, &vEndpoint);
  return 0;
}
