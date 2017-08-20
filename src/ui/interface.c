#include "ui/interface.h"

void uiStartup() {
    initscr();			/* Start curses mode 		  */
    printw("Midi Mapper");
    refresh();
    raw();
}

void uiShutdown() {
    endwin();
}


void MM_InterfaceStart() {
    MM_CreateVirtualDevice("midi-mapper");
    uiStartup();

    Devices *devices = MM_GetDevices();

    if (devices->count > 0) {
        printf("midi-mapper found devices:\n");

        for (int i = 0; i < devices->count; ++i) {
            printf(" %02d: %s\n", i, devices->store[i]->name);
        }

        void (*pFunc)(const MIDIPacketList *message, void *refCon, void *connRefCon);
        pFunc = &MM_MIDIReadProc;

        MM_AttachListener(devices->store[0], pFunc);

    } else {
        printf("midi-mapper: no midi devices available\n");
    }

    bool loop = true;
    while (loop) {
        sleep(10);
        loop = false;
    }

    uiShutdown();
}

