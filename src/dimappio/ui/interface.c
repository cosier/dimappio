#include "dimappio/ui/interface.h"

static void ui_startup() {
    initscr(); /* Start curses mode 		  */
    printw("Midi Mapper\n");
    raw();
}

static void ui_shutdown() { endwin(); }

void dm_interface_start() {
    ui_startup();
    ui_shutdown();
}
