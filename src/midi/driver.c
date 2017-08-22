#include "driver.h"

Devices *MM_GetDevices() {
#ifdef __APPLE__
  return MMCoreMidi_GetDevices();
#elif __linux__
  return MMAlsa_GetDevices();
#endif
}

MIDIClients *MM_GetClients() {
#ifdef __APPLE__
/* return MMCoreMidi_GetDevices(); */
#elif __linux__
  return MMAlsa_GetClients();
#endif
}

Device *MM_CreateVirtualDevice(char *name) {
#ifdef __APPLE__
  return MMCoreMidi_CreateVirtualDevice(name);
#elif __linux__
  return MMAlsa_CreateVirtualDevice(name);
#endif
}

bool MM_ClientExists(char *client) {
#ifdef __APPLE__
#TODO : implement.
#elif __linux__
  return MMAlsa_ClientExists(client);
#endif
}

void MM_ClientDetails(MIDIClient *client) {
#ifdef __APPLE__
#TODO : implement.
#elif __linux__
  MMAlsa_ClientDetails(client);
#endif
}

void MM_MonitorClient(char *client_with_port) {
#ifdef __APPLE__
#TODO : implement.
#elif __linux__
  MMAlsa_MonitorDevice(client_with_port);
#endif
}

void MM_ListClients() {
  MIDIClients *clients = MM_GetClients();

  for (int i = 0; i < clients->count; i++) {
    MM_ClientDetails(clients->store[i]);
  }
}

void MM_MIDINotifyProc(const MIDINotification *message, void *refCon) {
  // TODO:
}

void MM_MIDIReadProc(const MIDIPacketList *pktlist, void *refCon,
                     void *connRefCon) {
  // TODO:
}

void MM_AttachListener(Device *dev,
                       void (*func)(const MIDIPacketList *message, void *refcon,
                                    void *connRefCon)) {
  // TODO: implement
}

void MM_SendMidiNote(char *client, char *port, char *note) {
  printf("Sending Midi note -> %s:%s [%s]", client, port, note);
  // TODO: implement
}

void MM_DriverDebug() { MM_driver_debug_mode = true; }
