#!/bin/bash
set -euf -o pipefail

DIR="$( cd  "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT=$( cd $DIR/../ && pwd )

$ROOT/bin/clean.sh
mkdir -p $ROOT/build && cd $ROOT/build;

if [[ "$DEBUG" == true ]]; then
  BUILD=Debug
else
  BUILD=Release
fi

CMAKE_BUILD_TYPE=$BUILD $ROOT/bin/build.sh
cd $ROOT/build
make install
echo

