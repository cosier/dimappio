#!/bin/bash
set -euf -o pipefail

BIN="$( cd  "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $BIN/vars.sh

$BIN/clean.sh
DEBUG=true $BIN/build.sh

if [ ! $? -eq 0 ]; then
  echo "Cannot run Valgrind, build broken."
  exit 1
fi

if [[ $FULL == true ]]; then
  valgrind -v --show-reachable=yes --leak-check=full $EXE_BUILD $@
else
  valgrind -v --quiet --leak-check=full $EXE_BUILD $@
fi
