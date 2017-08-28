#include "ui/interface.h"

static void ui_startup() {
    initscr(); /* Start curses mode 		  */
    printw("Midi Mapper\n");
    raw();
}

static void ui_shutdown() { endwin(); }

void mm_interface_start() {
    ui_startup();
    ui_shutdown();
}
