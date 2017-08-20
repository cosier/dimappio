#include "ui/interface.h"

void uiStartup() {
  /* initscr(); /\* Start curses mode 		  *\/ */
  /* printw("Midi Mapper\n"); */
  /* raw(); */
}

void uiShutdown() {
  /* endwin(); */
}

void MM_InterfaceStart() {
  uiStartup();
  MM_CreateVirtualDevice("midi-mapper");

  Devices *devices = MM_GetDevices();

  if (devices->count > 0) {
    printf("midi-mapper found devices:\n");

    for (int i = 0; i < devices->count; ++i) {
      printf(" %02d: %s\n", i, devices->store[i]->name);
    }

    void (*pFunc)(const MIDIPacketList *message, void *refCon,
                  void *connRefCon);
    pFunc = &MM_MIDIReadProc;

    MM_AttachListener(devices->store[0], pFunc);

  } else {
    printf("no midi devices available\n");
  }

  /* bool loop = true; */
  /* while (loop) { */
  /*   sleep(10); */
  /*   /\* loop = false; *\/ */
  /* } */

  uiShutdown();
}
