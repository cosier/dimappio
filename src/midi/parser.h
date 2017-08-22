#ifndef __STDC_WANT_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#endif

#include <stdlib.h>
#include <string.h>

#include "utils.h"

typedef struct ClientPort {
  int client;
  int port;
} ClientPort;

ClientPort *parseStringToClientPort(char *client_with_port);
