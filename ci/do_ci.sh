#!/bin/bash

set -e

[ -z "${SRC_DIR}" ] && export SRC_DIR="`pwd`"
[ -z "${BUILD_DIR}" ] && export BUILD_DIR="`mktemp -d`"

if [[ "$1" == "cmake.test" ]]; then
  cd "${BUILD_DIR}"
  cmake "${SRC_DIR}"
  make
  make test
  exit 0
elif [[ "$1" == "cmake.plugin" ]]; then
  exit 0
else
  echo "Invalid do_ci.sh target"
  exit 1
fi
