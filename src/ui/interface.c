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

  pdebug("Querying Midi Clients");
  MidiClients *clients = MM_GetClients();

  if (clients->count > 0) {
    pdebug("\nmidi clients:\n");

    for (int i = 0; i < clients->count; ++i) {
      pdebug(" %d) %s (card: %d)", i,
             clients->store[i]->name,
             clients->store[i]->card
             );
    }

    /* MM_AttachListener(clients->store[0], &&MM_MIDIReadProc); */

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
