#include "device.h"

void mm_devices_free(mm_devices* devices) {
    for (int i = 0; i < devices->count; ++i) {
        if (devices->store[i] != NULL) {
            mm_device* dev = devices->store[i];
            if (dev->name != NULL) {
                free(dev->name);
            }

            free(devices->store[i]);
        }
    }
    free(devices->store);
    free(devices);
}
