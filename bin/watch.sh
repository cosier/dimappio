#!/bin/bash

BIN="$( cd  "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $BIN/vars.sh

function block_for_change() {
  inotifywait -q -r \
    -e close_write,moved_to,create,modify,delete,move \
    $ROOT/src
}

# Run initially
$BIN/run.sh clean build $@

# Now block and watch
while block_for_change; do
  echo "File changed: $filename"
  $BIN/run.sh clean build $@
done
