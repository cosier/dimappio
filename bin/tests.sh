#!/bin/bash
set -euf -o pipefail

BIN="$( cd  "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $BIN/vars.sh

# Build with debug flag on
DEBUG=true $BIN/build.sh

EXECUTE_SPECIFIC_TEST=${1:-}

# Don't run tests if we can't even build
if [[ $? != 0 ]]; then
  exit 1
fi

cd $BUILD_DIR

# Run the CTest suite
echo -e "\nmake test"
make test
failed=$?

if [[ "$EXECUTE_SPECIFIC_TEST" != "" ]]; then
  # Clear test results from the screen if they failed, as we are
  # already inspecting the tests for failure.
  if [[ $failed != 0 ]]; then
    clear
  fi

  echo -e "Running single test output: $1\n"
  $BUILD_DIR/tests/$1
fi

