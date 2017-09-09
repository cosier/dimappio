#include "dimappio/midi/device.h"

void dm_devices_free(dm_devices* devices) {
    for (int i = 0; i < devices->count; ++i) {
        if (devices->store[i] != NULL) {
            dm_device* dev = devices->store[i];
            if (dev->name != NULL) {
                free(dev->name);
            }

            free(devices->store[i]);
        }
    }
    free(devices->store);
    free(devices);
}
