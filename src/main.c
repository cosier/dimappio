#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "ui/interface.h"

int main(int argc, char **argv) {
  // Kick off the application UI thread
  MM_InterfaceStart();

  return 0;
}
