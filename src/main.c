#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

#include "ui/interface.h"


/* Flag set by ‘--verbose’. */
static int debug_flag;

void print_usage() {
    printf("Usage: midi-mapper\n");

    printf("  -m, --monitor = <id:port>  Monitor a MIDI Client\n");

    printf("  -d, --dump     Dump all available midi clients\n");
    printf("  -h, --help     Show application usage\n");
    printf("  -v, --version  Print version\n\n");
}

void print_version() {
    printf("midi-mapper v0.01\n");
    printf("Authors: Bailey Cosier <bailey@cosier.ca>\n");
    printf("Homepage: https://github.com/cosier/midi-mapper\n\n");
}

int main(int argc, char **argv) {
    int opt= 0;
    int monitor = -1, dump = -1, version = -1, help = -1;

    char *monitor_dev;

    //Specifying the expected options
    //The two options l and b expect numbers as argument
    static struct option long_options[] = {
        {"dump",      no_argument,       0,  'd' },
        {"help",      no_argument,       0,  'h' },
        {"version",   no_argument,       0,  'v' },
        {"monitor",   required_argument, 0,  'm' },
        {0,           0,                 0,  0   }
    };

    int long_index =0;
    while ((opt = getopt_long(argc, argv,"dhv:m:",
                              long_options, &long_index )) != -1) {
        switch (opt) {
        case 'd' :  dump = 0;
            break;
        case 'v' :  version = 0;
            break;
        case 'h' :  help = 0;
            break;
        case 'm' :  monitor = 0;
            monitor_dev = optarg;
            break;

        default: print_usage();
            exit(EXIT_FAILURE);
        }
    }

    if (version == 0) {
        print_version();
        exit(EXIT_SUCCESS);
    }

    if (help == 0) {
        print_usage();
        exit(EXIT_SUCCESS);
    }

    if (dump == 0) {
        MM_DumpAllClients(); 
        exit(EXIT_SUCCESS);
    }

    // Kick off the application UI thread
    /* MM_InterfaceStart(); */


    print_usage();
    return 0;
}
