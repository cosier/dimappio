#include <CoreAudio/HostTime.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#include <CoreMIDI/CoreMIDI.h>
#include <CoreMIDI/MIDIServices.h>

#include <AudioToolbox/AudioToolbox.h>

typedef struct Device {
  char* name;
} Device;

typedef struct Devices {
  Device** store;
  int count;
} Devices;


int CreateVirtualDevice();
Devices* GetMIDIDevices();
