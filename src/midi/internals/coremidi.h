#ifdef __APPLE__

#include <CoreAudio/HostTime.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#include <CoreMIDI/CoreMIDI.h>
#include <CoreMIDI/MIDIServices.h>

#include <AudioToolbox/AudioToolbox.h>

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

void MMCoreMidi_MIDIReadProc(const MIDIPacketList *pktlist, void *refCon,
                             void *connRefCon);
void MMCoreMidi_MIDINotifyProc(const MIDINotification *message, void *refCon);
void MMCoreMidi_AttachListener(Device dev,
                               void (*func)(const MIDINotification *message,
                                            void *refCon));

#endif
