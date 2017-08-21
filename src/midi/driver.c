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
  // TODO:
}
