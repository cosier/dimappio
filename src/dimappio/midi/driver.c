#include "driver.h"

dm_devices* dm_get_devices() {
#ifdef __APPLE__
    return dmc_get_devices();
#elif __linux__
    return dma_get_devices();
#endif
}

bool dm_client_exists(char* client) {
#ifdef __APPLE__
#TODO : implement.
#elif __linux__
    return dma_client_exists(client);
#endif
}

void dm_monitor_client(dm_options* options) {
#ifdef __APPLE__
// TODO : implement.
#elif __linux__
    dma_monitor_device(options);
#endif
}

void dm_event_loop(dm_options* options, dm_midi_output* output,
                   void (*render_callback)(dm_options* options,
                                           dm_key_node* tail,
                                           dm_key_set* key_set),
                   void (*key_callback)(int key, int ch, int vel, int on)) {
#ifdef __APPLE__
// TODO : implement.
#elif __linux__
    dma_event_loop(options, output, render_callback, key_callback);
#endif
}

void dm_list_clients() {
#ifdef __linux__
    dm_devices* devices = dma_get_devices();
#elif __APPLE__
// TODO: implement
#endif
    if (devices->count > 0) {
        printf("Device Ports found (%d):\n", devices->count);
    } else {
        printf("No MIDI Device Ports found to be available.\n\n");
    }

    for (int i = 0; i < devices->count; ++i) {
        dm_device* device = devices->store[i];
        char id[12];
        snprintf(id, sizeof(char) * 12, "[%d:%d]", device->client,
                 device->port);

        if (device->name != NULL) {
            printf("%-8s %s\n", id, device->name);
        } else {
            printf("%-8s\n", id);
        }
    }

    printf("\n");
}

dm_device* dm_get_midi_through() {
    dm_devices* devices = dm_get_devices();
    int size = sizeof(char*) * 64;

    for (int i = 0; i < devices->count; ++i) {
        if (devices->store[i]->name != NULL) {
            // char* n_dup = strdup(n);
            // for (int is = 0; n_dup[is]; ++is) {
            //     n_dup[is] = tolower(n_dup[is]);
            // }
            char* name = malloc(size);
            snprintf(name, size, "%s", devices->store[i]->name);

            char* match = strstr(name, "Midi Through");
            free(name);

            if (match != NULL) {
                // util_debug("dm_get_midi_through: found it: %s\n",
                //          devices->store[i]->name);
                int client = devices->store[i]->client;
                int port = devices->store[i]->port;

                dm_devices_free(devices);
                dm_device* dev = malloc(sizeof(dm_device));

                dev->client = client;
                dev->port = port;
                return dev;
            } else {
                // printf("dm_get_midi_through(): device not matched: %s\n",
                //        devices->store[i]->name);
                util_debug("dm_get_midi_through: not a match: %s\n",
                           devices->store[i]->name);
            }
        }
    }

    util_debug("dm_get_midi_through: not found\n");
    dm_devices_free(devices);
    return NULL;
}

void dm_send_midi_to_client(int client, int port, char* note, bool on,
                            int channel, int vel) {
    printf("dm_send_midi_to_client -> %d:%d [%s, %d, ch:%d, vel:%d]", client,
           port, note, on, channel, vel);
#ifdef __linux__
    dma_send_midi_note(client, port, note, on, channel, vel);
#elif __APPLE__
// TODO:
#endif
}

void dm_send_midi(dm_midi_output* output, int midi, bool on, int ch, int vel) {
#ifdef __linux__
    dma_send_midi(output, midi, on, ch, vel);
#elif __APPLE__
// TODO:
#endif
}
void dm_send_event(dm_midi_output* output, dm_midi_event* ev) {
#ifdef __linux__
    dma_send_event(output, ev);
#elif __APPLE__
// TODO:
#endif
}

void dm_driver_init(dm_midi_device** dev, char* name) {
#ifdef __linux__
    dma_init_sequencer(dev, name);
#elif __APPLE__
// TODO:
#endif
}

void dm_send_events_to(dm_midi_output* output, int client, int port) {
#ifdef __linux__
    dma_send_events_to(output, client, port);
#elif __APPLE__
// TODO:
#endif
}

void dm_receive_events_from(dm_midi_output* output, int client, int port) {
#ifdef __linux__
    dma_receive_events_from(output, client, port);
#elif __APPLE__
// TODO:
#endif
}

void dm_driver_debug() { dm_driver_debug_mode = true; }
