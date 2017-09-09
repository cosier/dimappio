#!/bin/bash
#!/bin/bash
set -euf -o pipefail

BIN="$( cd  "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT=$( cd $BIN/../ && pwd )

source $BIN/vars.sh

if [ -d $BUILD_DIR ]; then
  rm -rf $BUILD_DIR
fi
