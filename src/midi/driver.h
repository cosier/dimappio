#include <CoreAudio/HostTime.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#include <CoreMIDI/CoreMIDI.h>
#include <CoreMIDI/MIDIServices.h>

#include <AudioToolbox/AudioToolbox.h>

#import "utils.h"

typedef struct Device {
  char *name;
  MIDIEndpointRef endpoint;
} Device;

typedef struct Devices {
  Device **store;
  int count;
} Devices;

extern int MMCreateVirtualDevice();
extern Devices *MMGetDevices();

void MMMIDIReadProc(const MIDIPacketList *pktlist, void *refCon, void *connRefCon);
void MMMIDINotifyProc(const MIDINotification *message, void *refCon);
void MMAttachListener();
