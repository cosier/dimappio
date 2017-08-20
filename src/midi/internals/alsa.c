#include "midi/internals/alsa.h"
#ifdef __linux__
#include <stdlib.h>

Devices *MMAlsa_GetDevices() {
  Devices *devices = malloc(sizeof(Devices));

  devices->count = 0;
  return devices;
}

#endif
