#include "ui/interface.h"

static void ui_startup() {
    /* initscr(); /\* Start curses mode 		  *\/ */
    /* printw("Midi Mapper\n"); */
    /* raw(); */
}

static void ui_shutdown() { /* endwin(); */
}

void mm_interface_start() {
    ui_startup();
    mm_create_virtual_device("midi-mapper");

    MIDIClients* clients = mm_get_clients(NULL);

    if (clients->count > 0) {
        for (int i = 0; i < clients->count; ++i) {
            mm_client_details(clients->store[i]);
        }

        /* mm_attach_listener(clients->store[0], &&MM_MIDIReadProc); */

    } else {
        printf("no midi devices available\n");
    }

    /* bool loop = true; */
    /* while (loop) { */
    /*   sleep(10); */
    /*   /\* loop = false; *\/ */
    /* } */

    /* ui_shutdown(); */
}
