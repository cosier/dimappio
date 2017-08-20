#ifdef __APPLE__

void MMCoreMidi_MIDIReadProc(const MIDIPacketList *pktlist, void *refCon,
                             void *connRefCon) {

  Device *dev = (Device *)refCon;
  printf("received midi: %d\n", dev->name);
}

void MMCoreMidi_MIDINotifyProc(const MIDINotification *message, void *refCon) {
  Device *dev = (Device *)refCon;
  printf("MIDI Notify -> messageID=%d", message->messageID);
}

void MMCoreMidi_AttachListener(Device dev,
                               void (*func)(const MIDINotification *message,
                                            void *refCon)) {}

Devices *MMCoreMidi_GetDevices() {
  int srcs = MIDIGetNumberOfSources();
  /* printf("MIDI Sources: %d\n", srcs); */

  if (srcs < 1) {
    printf("No Midi Sources found, attempting to force enumeration(10)\n");
    srcs = 10;
  }

  // Device Sentinel
  Devices *devices = NULL;

  // Allocate master device sentinel
  devices = malloc(sizeof(Devices *));
  devices->count = 0;

  // Allocate a collection of pointers to Device pointers
  devices->store = malloc(srcs * sizeof(Device *));

  for (int i = 0; i < srcs; ++i) {
    devices->store[i] = (Device *)malloc(sizeof(Device));
    /* MIDIDeviceRef dev = MIDIGetDevice(i); */
    MIDIDeviceRef src = MIDIGetSource(i);

    if (src) {
      CFPropertyListRef *propList = NULL;
      MIDIObjectGetProperties(src, propList, true);

      CFStringRef deviceName = NULL;
      MIDIObjectGetStringProperty(src, kMIDIPropertyName, &deviceName);

      devices->store[i]->endpoint = src;
      devices->store[i]->name = CFStringRefToChars(deviceName);

      // Increment device counter
      devices->count++;
    }
  }

  return devices;
}

int MMCoreMidi_CreateVirtualDevice(char *cname) {
  CFStringRef name = CharToCFStringRef(cname);

  MIDIClientRef client;
  MIDIEndpointRef endpoint;

  MIDIPortRef *output = NULL;
  MIDIPortRef *input = NULL;

  Connection con;
  con.id = 1;

  Device dev;
  dev.name = cname;

  MIDIClientCreate(name, MMMIDINotifyProc, &dev, &client);

  MIDIOutputPortCreate(client, CFSTR("output"), output);
  MIDIInputPortCreate(client, CFSTR("input"), MMMIDIReadProc, &dev, output);

  MIDIDestinationCreate(client, name, MMMIDIReadProc, &dev, &endpoint);
  dev.endpoint = endpoint;

  MIDISourceCreate(client, name, &endpoint);
  return 0;
}

#endif
