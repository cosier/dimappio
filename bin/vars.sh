#!/bin/bash
set -euf -o pipefail

BIN="$( cd  "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT=$( cd $BIN/../ && pwd )
BUILD_DIR=$ROOT/build

# Apply some cat sed grep trickery to extract
# APP_LIB_NAME and APP_EXE_NAME from CMake config.
APP_LIB_NAME=$(cat $ROOT/CMakeLists.txt \
  | grep -m1 APP_LIB_NAME | sed 's/[a-z\s(].*APP_LIB_NAME\s\(.*\))/\1/')

APP_EXE_NAME=$(cat $ROOT/CMakeLists.txt \
  | grep -m1 APP_EXE_NAME | sed 's/[a-z\s(].*\APP_EXE_NAME\s\(.*\))/\1/')

EXE_NAME=$APP_EXE_NAME
EXE_BUILD=$BUILD_DIR/src/main/$EXE_NAME

