#include "driver.h"

Devices* mm_get_devices() {
#ifdef __APPLE__
    return mmc_get_devices();
#elif __linux__
    return mma_get_devices();
#endif
}

MIDIClients* mm_get_clients() {
#ifdef __APPLE__
/* return mmc_get_devices(); */
#elif __linux__
    return mma_get_clients(NULL);
#endif
}

Device* mm_create_virtual_device(char* name) {
#ifdef __APPLE__
    return mmc_create_virtual_device(name);
#elif __linux__
    return mma_create_virtual_device(name);
#endif
}

bool mm_client_exists(char* client) {
#ifdef __APPLE__
#TODO : implement.
#elif __linux__
    return mma_client_exists(client);
#endif
}

void mm_client_details(MIDIClient* client) {
#ifdef __APPLE__
#TODO : implement.
#elif __linux__
    mma_client_details(client);
#endif
}

void mm_monitor_client(char* client_with_port, mm_mapping* mappings) {
#ifdef __APPLE__
#TODO : implement.
#elif __linux__
    mma_monitor_device(client_with_port, mappings);
#endif
}

void mm_list_clients() {
    MIDIClients* clients = mm_get_clients(NULL);

    for (int i = 0; i < clients->count; i++) {
        mm_client_details(clients->store[i]);
    }
}

void mm_attach_listener(Device* dev,
                        void (*func)(const MIDIPacketList* message,
                                     void* refcon, void* connRefCon)) {
    // TODO: implement
}

void mm_send_midi_note(int client, int port, char* note, bool on, int channel,
                       int vel) {
    printf("mm_send_midi_note -> %d:%d [%s]", client, port, note);
    mma_send_midi_note(client, port, note, on, channel, vel);
}

void mm_driver_debug() { mm_driver_debug_mode = true; }
