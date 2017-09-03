#include <getopt.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "core.h"
#include "version.h"

void print_usage() {
    printf("Usage: dimappio [options] [Mapping]\n\n");
    printf("[Mapping] consists of a comma delimited list of notes.\n");
    printf("Within that list, you may nest sub lists to specify \na group of "
           "notes to be executed per src trigger\n");

    printf("\nExample:\n");
    printf("dimappio -x 32:0  C3:C3|C4|C5,F#3:F#4\n");

    printf("\nThis will create a virtual interface attached to the device port "
           "32:0.\n");
    printf("Then it will map two keys (C3 and F#3) to trigger multiple other "
           "keys\n");

    printf("\nOptions:\n");
    printf("  -s, --send=<note>       Send midi note to a specifc client\n");
    printf("  -t, --target=<id:port>  Specify a target Midi Client\n");
    printf("  -x, --source=<id:port>  Specify a Midi Client source\n");
    printf("\n");

    printf("  -m, --monitor           Monitor a MIDI Client\n");
    printf("  -l, --list     List all available midi clients\n");
    printf("  -z, --debug    Turn on debug mode\n");
    printf("\n");

    printf("  -h, --help     Show application usage\n");
    printf("  -v, --version  Print version\n\n");
}

void print_version() {
    printf("Di'Mappio v%s.%s.%s\nrevision: %s\nbuild date: %s\n\n",
           MM_VERSION_MAJOR, MM_VERSION_MINOR, MM_VERSION_PATCH,
           MM_VERSION_BUILD, MM_VERSION_DATE);

    printf("Authors: Bailey Cosier <bailey@cosier.ca>\n");
    printf("Homepage: https://github.com/cosier/dimappio\n\n");
}

int requires_target_specified(char* cmd) {
    printf("--%s command requires a target to be specified.\n", cmd);
    printf("Use the -t,--target=<id:port> argument to specify a target\n\n");
    print_usage();

    exit(EXIT_FAILURE);
}

int requires_source_specified(char* cmd) {
    printf("--%s command requires a Midi Client Source to be specified.\n",
           cmd);
    printf(
        "Use the -s,--source=<id:port> argument to specify a Midi source\n\n");
    print_usage();

    exit(EXIT_FAILURE);
}

bool verify_valid_midi_client(char* client) {
    if (strstr(client, ":") == NULL) {
        printf("Midi Client (%s) does not contain port specification \":\".\n",
               client);

        printf("For example, here is a valid Midi Client: \"123:0\"\n\n");
        return false;
    }

    if (!mm_client_exists(client)) {
        printf("Midi Client (%s) was not found or is no longer available.\n\n",
               client);
        printf("Try \"dimappio --list\" to list available Midi Clients.\n");
        printf("The [xx:x] denotes the client id + port number.\n\n");
        return false;
    }

    return true;
}

void handle_invalid_send_args(int argc, int optind) {
    fprintf(stderr, "Send requires 4 parameters: (note) (ch) "
                    "(velocity) (on/off)\nargc/optind [%d/%d] \n\n",
            argc, optind);
    print_usage();
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
    int opt = 0;

    bool list = false, version = false, help = false, monitor = false,
         debug = false;

    char* target = NULL;
    char* source = NULL;
    char* send_note = NULL;
    char* mapsrc = NULL;

    mm_mapping* mapping = NULL;

    int note_ch = 0;
    int note_vel = 0;
    int keys = 49;

    char* note_on_str;
    bool note_on = false;

    // Specifying the expected options
    // The two options l and b expect numbers as argument
    static struct option long_options[] = {
        {"list", no_argument, 0, 'l'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {"debug", no_argument, 0, 'z'},
        {"monitor", no_argument, 0, 'm'},
        {"keys", required_argument, 0, 'k'},
        {"remap", required_argument, 0, 'r'},
        {"send", required_argument, 0, 's'},
        {"target", required_argument, 0, 't'},
        {"source", required_argument, 0, 'x'},
        {0, 0, 0, 0}};

    int long_index = 0;
    while ((opt = getopt_long(argc, argv, "lhvzmr:t:k:x:s:", long_options,
                              &long_index)) != -1) {

        switch (opt) {
        case 'l':
            list = true;
            break;

        case 'v':
            version = true;
            break;

        case 'h':
            help = true;
            break;

        case 'm':
            monitor = true;
            break;

        case 'z':
            debug = true;
            break;

        case 'k':
            keys = atoi(optarg);
            break;

        case 't':
            target = optarg;
            break;

        case 's': {
            int provided = argc - optind;
            bool invalid_send_args = false;

            if (provided < 3) {
                handle_invalid_send_args(argc, optind);
            }

            note_ch = atoi(argv[optind]);
            note_vel = atoi(argv[optind + 1]);
            note_on_str = argv[optind + 2];

            if (strcmp(note_on_str, "on") == 0) {
                note_on = true;
            } else if (strcmp(note_on_str, "off") == 0) {
                note_on = false;
            } else {
                fprintf(stderr, "Invalid note on/off paramater.\nAccepted "
                                "options are: 'on' or 'off'. Found %s (%d)\n\n",
                        note_on_str, optind);
                handle_invalid_send_args(argc, optind);
            }

            send_note = optarg;

            break;
        }

        case 'x':
            source = optarg;
            break;

        case 'r':
            mapsrc = optarg;
            break;

        default:
            print_usage();
            return 1;
        }
    }

    // Capture n amount of mappings provided to the CLI
    if (optind < argc) {
        char* mapbuf = malloc(sizeof(char*) * 1024);
        mapbuf[0] = '\0';
        int opts = argc - optind;
        for (int i = 0; i < opts; ++i) {
            if (i > 0) {
                sprintf(mapbuf, "%s,%s", mapbuf, argv[optind + i]);
            } else {
                sprintf(mapbuf, "%s", argv[optind + i]);
            }
        }
        mapsrc = mapbuf;
    }

    // Flip global debug flag.
    if (debug) {
        mm_driver_debug();
        mm_debug("\033c[midi-mapper started]: %lu\n", mm_micros());
    }

    // Version output and then exit.
    if (version) {
        print_version();
        return 0;
    }

    // Show cli help usage and exit.
    if (help) {
        print_usage();
        return 0;
    }

    // List available midi client ports then exit.
    if (list) {
        mm_list_clients();
        return 0;
    }

    mm_device* midi_through = mm_get_midi_through();

    // If target client:port is specified, verify that it exists.
    if (target != NULL) {
        if (!verify_valid_midi_client(target)) {
            exit(EXIT_FAILURE);
        }
    } else if (midi_through != NULL) {
        target = malloc(sizeof(char*) * 6);
        sprintf(target, "%d:%d", midi_through->client, midi_through->port);
    }

    // Done with midi_through fallback, time to free
    if (midi_through != NULL) {
        free(midi_through);
    }

    // If source client:port is specified, verify that it exists.
    if (source != NULL) {
        if (!verify_valid_midi_client(source)) {
            exit(EXIT_FAILURE);
        }
    }

    // Send a midi note to a specific client.
    if (send_note != NULL) {
        mm_debug("main: attempting to send midi note");
        if (target == NULL) {
            requires_target_specified("send");
        }

        mm_device* cp = mm_parse_device(target);
        mm_debug("main: sending midi note to: %s", target);
        mm_send_midi_note(cp->client, cp->port, send_note, note_on, note_ch,
                          note_vel);
        return 0;
    }

    // If mappings are provided, check for a valid target and source.
    // Build a MapDefinition struct from cli remap definition.
    if (mapsrc != NULL) {
        if (source == NULL) {
            requires_source_specified("remap");
        } else {
            if (!verify_valid_midi_client(source)) {
                exit(EXIT_FAILURE);
            }
        }

        // Build mappings from a comma delimited string.
        mm_debug("main: attempting to build mappings from list using: %s\n",
                 mapsrc);
        mapping = mm_mapping_from_list(mapsrc);
        free(mapsrc);
    } else {
        mm_debug("main: building solo mappings\n");
        mapping = mm_build_mapping();
    }

    if (mapping == NULL) {
        error("Invalid Mapping provided\n");
        printf("-----------------------------------\n");
        print_usage();
        exit(EXIT_FAILURE);
    } else {
        mm_debug("main: mappings created successfully\n");
    }

    if (mapping->group_count) {
        char* buf = malloc(sizeof(char) * 128 * mapping->group_count);
        mm_mapping_dump(mapping, buf);
        printf("%s\n%s▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄%s\n ",
               buf, BLACK, RESET);
        free(buf);
    }

    mm_options* options = mm_create_options();
    options->mapping = mapping;
    options->keys = keys;
    options->source = source;
    options->target = target;

    // Engage in monitor loop.
    // Uses a source to poll events from.
    //
    // Sourced events are passed through the remap filter
    // before being broadcast to any subscribers.
    /* if (monitor || mapsrc != NULL) { */
    if (monitor || true) {
        if (source == NULL) {
            requires_source_specified("monitor");
        }

        mm_debug("main: attempting to monitor client\n");
        mm_monitor_client(options);
        exit(EXIT_SUCCESS);
    }

    print_usage();
    return 0;
}
