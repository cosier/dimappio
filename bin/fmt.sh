#!/bin/bash
# Format source code in the entire tree based on clang-format
set -euf -o pipefail

DIR="$( cd  "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $BIN/vars.sh

SRC=$ROOT/src

# Function to take input from stdout in the proceeding pipe
cfmt() {
  declare INPUT=${@:-$(</dev/stdin)}
  for f in $INPUT; do
    echo $f
    clang-format -i -style=file $f
  done
}

find $SRC  -type f \( -iname "*.h" -o -iname "*.c" ! -iname "*#*"  \) | cfmt
