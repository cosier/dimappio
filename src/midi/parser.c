#include "midi/parser.h"

ClientPort* parse_client_port(char* client_with_port) {
    ClientPort* cp = malloc(sizeof(ClientPort));
    cp->client = 0;
    cp->port = 0;

    if (strstr(client_with_port, ":") == NULL) {
        error("client_with_port(%s) does not contain \":\" delimiter",
              client_with_port);
        return NULL;
    }

    char* container;
    client_with_port = strdup(client_with_port);
    char* token = strtok_r(client_with_port, ":", &container);

    if (token != NULL) {
        cp->client = atoi(token);
    } else {
        error("parsing of (%s) failed due to client token", client_with_port);
    }

    token = strtok_r(NULL, "", &container);

    if (token != NULL) {
        cp->port = atoi(token);
    } else {
        error("parsing of (%s) failed due to port token", client_with_port);
    }

    return cp;
}
