#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "midi/driver.h"

int main ( int argc, char **argv ) {
  int r = CreateVirtualDevice();
  printf("\nmidimapper: initialised\n\n");

  while (true) {
    printf("\033[A\33[2K\rmidimapper: active -> %ld\n", time(0));
    fflush(stdout);

    sleep(1);
  }

  return 0;
}
