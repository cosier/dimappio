#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

#include "ui/interface.h"

void print_usage() {
    printf("Usage: midi-mapper\n");

    printf("  -m, --monitor = <id:port> Monitor a MIDI Client\n");
    printf("  -t, --target  = <id:port> Specify a target Midi Client (use with --send-note)\n");
    printf("  -x, --source = <id:port> Specify a Midi Client Source for mapping from\n");
    printf("  -s, --send = <note> Send midi note to a specifc client\n");
    printf("  -r, --remap = <note:note> Remap a list of note<>note\n");

    printf("\n");
    printf("  -l, --list     List all available midi clients\n");
    printf("  -h, --help     Show application usage\n");
    printf("  -v, --version  Print version\n\n");
}

void print_version() {
    printf("midi-mapper v0.01\n");
    printf("Authors: Bailey Cosier <bailey@cosier.ca>\n");
    printf("Homepage: https://github.com/cosier/midi-mapper\n\n");
}

int requires_target_specified(char *cmd) {
    printf("--%s command requires a target to be specified.\n", cmd);
    printf("Use the -t,--target=<id:port> argument to specify a target\n\n");
    print_usage();

    exit(EXIT_FAILURE);
}

int requires_source_specified(char *cmd) {
    printf("--%s command requires a Midi Client Source to be specified.\n", cmd);
    printf("Use the -s,--source=<id:port> argument to specify a Midi source\n\n");
    print_usage();

    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    int opt = 0;

    int list       = 0,
        version    = 0,
        help       = 0;


    char *monitor   = NULL;
    char *target    = NULL;
    char *source    = NULL;
    char *send_note = NULL;
    char *mapping   = NULL;

    //Specifying the expected options
    //The two options l and b expect numbers as argument
    static struct option long_options[] = {
        {"list",      no_argument,       0,  'l' },
        {"help",      no_argument,       0,  'h' },
        {"version",   no_argument,       0,  'v' },
        {"monitor",   required_argument, 0,  'm' },
        {"remap",     required_argument, 0,  'r' },
        {"send",      required_argument, 0,  's' },
        {"target",    required_argument, 0,  't' },
        {0,           0,                 0,  0   }
    };

    int long_index =0;
    while ((opt = getopt_long(argc, argv,"lhv:m:",
                              long_options, &long_index )) != -1) {
        switch (opt) {
        case 'l' :  list = 1;
            break;
        case 'v' :  version = 1;
            break;
        case 'h' :  help = 1;
            break;

        case 's' :
            send_note = optarg;
            break;

        case 'x' :
            source = optarg;
            break;

        case 'r' :
            mapping = optarg;
            break;

        case 'm' :
            monitor = optarg;
            break;

        case 't' :
            target = optarg;
            break;

        default: print_usage();
            exit(EXIT_FAILURE);
        }
    }

    if (version) {
        print_version();
        exit(EXIT_SUCCESS);
    }

    if (help) {
        print_usage();
        exit(EXIT_SUCCESS);
    }

    if (list) {
        MM_ListClients();
        exit(EXIT_SUCCESS);
    }

    if (target != NULL) {
        // TODO: verify target correctness (id:port)
    }

    if (mapping != NULL) {
        if (target == NULL) {
            requires_target_specified("mapping");
        }

        if (source == NULL) {
            requires_source_specified("remap");
        }

        // TODO: implement remapping from source to target
    }

    if (send_note != NULL) {
        if (target == NULL) {
            requires_target_specified("send");
        }

        char *target_client = "30";
        char *target_port = "0";

        MM_SendMidiNote(target_client, target_port, send_note);
        exit(EXIT_SUCCESS);
    }

    // Kick off the application UI thread
    /* MM_InterfaceStart(); */

    print_usage();
    return 0;
}
