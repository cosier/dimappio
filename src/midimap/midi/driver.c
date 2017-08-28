#include "driver.h"

mm_devices* mm_get_devices() {
#ifdef __APPLE__
    return mmc_get_devices();
#elif __linux__
    return mma_get_devices();
#endif
}

bool mm_client_exists(char* client) {
#ifdef __APPLE__
#TODO : implement.
#elif __linux__
    return mma_client_exists(client);
#endif
}

void mm_monitor_client(char* source, char* target, mm_mapping* mappings) {
#ifdef __APPLE__
#TODO : implement.
#elif __linux__
    mma_monitor_device(source, target, mappings);
#endif
}

void mm_list_clients() {}

void mm_send_midi_note(int client, int port, char* note, bool on, int channel,
                       int vel) {
    printf("mm_send_midi_note -> %d:%d [%s]", client, port, note);
    mma_send_midi_note(client, port, note, on, channel, vel);
}

void mm_driver_init(mm_midi_device** dev, char* name) {
#ifdef __linux__
    mma_init_sequencer(dev, name);
#elif __APPLE__
#endif
}

void mm_send_events_to(mm_midi_output* output, int client, int port) {
#ifdef __linux__
    mma_send_events_to(output, client, port);
#elif __APPLE__
#endif
}

void mm_receive_events_from(mm_midi_output* output, int client, int port) {
#ifdef __linux__
    mma_receive_events_from(output, client, port);
#elif __APPLE__
#endif
}

void mm_driver_debug() { mm_driver_debug_mode = true; }
