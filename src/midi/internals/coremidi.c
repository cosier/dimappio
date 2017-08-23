#ifdef __APPLE__
#include "midi/internals/coremidi.h"
#include <stdlib.h>

void mm_midi_read_proc(const MIDIPacketList* pktlist, void* refCon,
                             void* connRefCon) {

    Device* dev = (Device*)refCon;
    printf("received midi: %s\n", dev->name);
}

void mmc_midi_notify_proc(const MIDINotification* message, void* refCon) {
    Device* dev = (Device*)refCon;
    printf("MIDI Notify -> messageID=%d", message->messageID);
}

void mmc_attach_listener(Device dev,
                               void (*func)(const MIDINotification* message,
                                            void* refCon)) {}

Devices* mmc_get_devices() {
    int srcs = MIDIGetNumberOfSources();
    /* printf("MIDI Sources: %d\n", srcs); */

    if (srcs < 1) {
        printf("No Midi Sources found, attempting to force enumeration(10)\n");
        srcs = 10;
    }

    // Device Sentinel
    Devices* devices = NULL;

    // Allocate master device sentinel
    devices = malloc(sizeof(Devices));
    devices->count = 0;

    // Allocate a collection of pointers to Device pointers
    devices->store = malloc(srcs * sizeof(Device*));

    for (int i = 0; i < srcs; ++i) {
        // Allocate actual device here
        devices->store[i] = malloc(sizeof(Device));
        /* MIDIDeviceRef dev = MIDIGetDevice(i); */
        MIDIDeviceRef src = MIDIGetSource(i);

        if (src) {
            CFPropertyListRef* propList = NULL;
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

Device* mmc_create_virtual_device(char* cname) {
    CFStringRef name = CharToCFStringRef(cname);

    MIDIClientRef client;
    MIDIEndpointRef endpoint;

    MIDIPortRef* output = NULL;
    MIDIPortRef* input = NULL;

    Device dev;
    dev.name = cname;

    MIDIClientCreate(name, mm_midi_notify_proc, &dev, &client);

    MIDIOutputPortCreate(client, CFSTR("output"), output);
    MIDIInputPortCreate(client, CFSTR("input"), mm_midi_read_proc, &dev, output);

    MIDIDestinationCreate(client, name, mm_midi_read_proc, &dev, &endpoint);
    dev.endpoint = endpoint;

    MIDISourceCreate(client, name, &endpoint);
    return 0;
}

#endif
