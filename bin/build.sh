#!/bin/bash

BIN="$( cd  "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT=$( cd $BIN/../ && pwd )

source $BIN/vars.sh

# Ensure build dir exists and change directory
mkdir -p $BUILD_DIR && cd $BUILD_DIR;

RELEASE=${RELEASE:-}
DEBUG=${DEBUG:-}
BUILD_TYPE=${CMAKE_BUILD_TYPE:-}

if [[ "$RELEASE" == true ]]; then
  BUILD_TYPE=Release
elif [[ "$DEBUG" == true ]]; then
  BUILD_TYPE=Debug
fi

# Fallback to Debug build if no explicit type found
if [ -z "$BUILD_TYPE" ]; then
 BUILD_TYPE=Debug
fi

export BUILD_REVISION=$(git rev-parse --short --verify HEAD)
export BUILD_TIMESTAMP=$(date)

if [[ "$BUILD_REVISION" == "" ]]; then
  echo "Invalid Build Revision: $BUILD_REVISION"
  exit 1
fi

echo
echo -e " APP_LIB_NAME:    $APP_LIB_NAME"
echo -e " APP_EXE_NAME:    $APP_EXE_NAME"
echo -e " BUILD_TYPE:      $BUILD_TYPE"
echo -e " BUILD_REVISION:  $BUILD_REVISION "
echo -e " BUILD_TIMESTAMP: $BUILD_TIMESTAMP"
echo -e "-------------------------------------\n"

# If Makefile doesn't exist, initiate cmake to generate it.
if [[ ! -f $BUILD_DIR/Makefile ]]; then
  cmake \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    ..

  if [ ! $? -eq 0 ]; then
    exit 1
  fi
fi

# Run the build compiler
make -j4
if [ ! $? -eq 0 ]; then
  echo "Make failed"
  exit 1
fi

