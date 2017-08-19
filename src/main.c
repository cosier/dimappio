#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "midi/driver.h"

int main(int argc, char** argv)
{

    if (argc > 2 || argc < 2) {
        printf(
            "Usage:\n  %s <midi device name>\n", argv[0]);
        return 1;
    }

    Devices *devices = GetMIDIDevices();
    printf("midimapper found devices:\n");

    for (int i = 0; i < devices->count; ++i) {
      printf(" %d: %s\n", i, devices->store[i]->name);
    }

    char* name = argv[1];
    CreateVirtualDevice(name);

    while (true) {
        sleep(1);
    }

    return 0;
}
