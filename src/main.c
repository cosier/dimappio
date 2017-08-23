#include <getopt.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "core.h"

void print_usage() {
    printf("Usage: midi-mapper\n");

    printf("  -r, --remap=<note:note> Remap a list (comma delimited) of "
           "note<>note\n");
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
    printf("midi-mapper v0.01\n");
    printf("Authors: Bailey Cosier <bailey@cosier.ca>\n");
    printf("Homepage: https://github.com/cosier/midi-mapper\n\n");
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
        printf("Try \"midi-mapper --list\" to list available Midi Clients.\n");
        printf("The [xx:x] denotes the client id + port number.\n\n");
        return false;
    }

    return true;
}

int main(int argc, char** argv) {
    int opt = 0;

    bool list = false,
        version = false,
        help = false,
        monitor = false,
        debug = false;

    char* target = NULL;
    char* source = NULL;
    char* send_note = NULL;
    char* mapping = NULL;

    mm_mapping *mappings = NULL;

    // Specifying the expected options
    // The two options l and b expect numbers as argument
    static struct option long_options[] = {
        {"list", no_argument, 0, 'l'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {"debug", no_argument, 0, 'z'},
        {"monitor", no_argument, 0, 'm'},
        {"remap", required_argument, 0, 'r'},
        {"send", required_argument, 0, 's'},
        {"target", required_argument, 0, 't'},
        {"source", required_argument, 0, 'x'},
        {0, 0, 0, 0}};

    int long_index = 0;
    while ((opt = getopt_long(argc, argv, "lhvzmr:s:t:x:", long_options,
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

        case 't':
            target = optarg;
            break;

        case 's':
            send_note = optarg;
            break;

        case 'x':
            source = optarg;
            break;

        case 'r':
            mapping = optarg;
            break;

        default:
            print_usage();
            exit(EXIT_FAILURE);
        }
    }

    // Flip global debug flag.
    if (debug) {
        mm_driver_debug();
    }

    // Version output and then exit.
    if (version) {
        print_version();
        exit(EXIT_SUCCESS);
    }

    // Show cli help usage and exit.
    if (help) {
        print_usage();
        exit(EXIT_SUCCESS);
    }

    // List available midi client ports then exit.
    if (list) {
        mm_list_clients();
        exit(EXIT_SUCCESS);
    }

    // If target client:port is specified, verify that it exists.
    if (target != NULL) {
        if (!verify_valid_midi_client(target)) {
            exit(EXIT_FAILURE);
        }
    }

    // If source client:port is specified, verify that it exists.
    if (source != NULL) {
        if (!verify_valid_midi_client(source)) {
            exit(EXIT_FAILURE);
        }
    }

    // Send a midi note to a specific client.
    if (send_note != NULL) {
        if (target == NULL) {
            requires_target_specified("send");
        }

        char* target_client = "30";
        char* target_port = "0";

        mm_send_midi_note(target_client, target_port, send_note);
        exit(EXIT_SUCCESS);
    }

    // If mappings are provided, check for a valid target and source.
    // Build a MapDefinition struct from cli remap definition.
    if (mapping != NULL) {
        if (source == NULL) {
            requires_source_specified("remap");
        } else {
            if (!verify_valid_midi_client(source)) {
                exit(EXIT_FAILURE);
            }
        }

        // Build mappings from a comma delimited string.
        mappings = mm_mapping_from_list(mapping);

        // TODO: optional, build mappings from a file (ie. yaml parser)
    } else {
        mappings = mm_build_mapping();
    }


    // Engage in monitor loop.
    // Uses a source to poll events from.
    //
    // Sourced events are passed through the remap filter
    // before being broadcast to any subscribers.
    if (monitor) {
        if (source == NULL) {
            requires_source_specified("monitor");
        }

        mm_monitor_client(source, mappings);
        exit(EXIT_SUCCESS);
    }

    // Kick off the application UI thread
    /* mm_interface_start(); */

    print_usage();
    return 0;
}
