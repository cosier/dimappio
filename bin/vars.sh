#!/bin/bash
set -euf -o pipefail

BIN="$( cd  "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT=$( cd $BIN/../ && pwd )
BUILD_DIR=$ROOT/build

APP_LIB_NAME=dimappio
APP_EXE_NAME=dmappio

EXE_NAME=$APP_EXE_NAME
EXE_BUILD=$BUILD_DIR/src/main/$EXE_NAME

