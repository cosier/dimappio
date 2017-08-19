#include "driver.h"

typedef struct Connection {
  int id;
} Connection;

typedef struct MIDIPlayer {
  AUGraph graph;
  AudioUnit instrumentUnit;
} MIDIPlayer;

void MyMIDIReadProc(const MIDIPacketList *pktlist, void *refCon,
                    void *connRefCon) {

  Connection *c = (Connection *)refCon;
  printf("received midi: %d\n", c->id);
}

void MyMIDINotifyProc(const MIDINotification *message, void *refCon) {
  printf("MIDI Notify -> messageID=%d", message->messageID);
  Connection *c = (Connection *)refCon;
}

Devices *GetMIDIDevices() {
  int devs = 12;

  // Device Sentinel
  Devices *devices = NULL;

  // Allocate master device sentinel
  devices = malloc(sizeof(Devices *));

  // Allocate a collection of pointers to Device pointers
  devices->store = malloc(devs * sizeof(Device *));

  for (int i = 0; i < devs; ++i) {
    devices->store[i] = (Device *)malloc(sizeof(Device));
    devices->store[i]->name = strdup("ok");
    devices->count = i;
  }

  return devices;
}

int CreateVirtualDevice(char *cname) {
  CFStringRef name;
  name = CFStringCreateWithCStringNoCopy(NULL, cname, kCFStringEncodingMacRoman,
                                         NULL);

  MIDIClientRef client;
  MIDIEndpointRef endpoint;

  MIDIPortRef *output = NULL;
  MIDIPortRef *input = NULL;

  Connection con;
  con.id = 1;

  MIDIClientCreate(name, MyMIDINotifyProc, &con, &client);

  MIDIOutputPortCreate(client, CFSTR("output"), output);
  MIDIInputPortCreate(client, CFSTR("input"), MyMIDIReadProc, &con, output);

  MIDIDestinationCreate(client, name, MyMIDIReadProc, &con, &endpoint);
  MIDISourceCreate(client, name, &endpoint);
  return 0;
}
