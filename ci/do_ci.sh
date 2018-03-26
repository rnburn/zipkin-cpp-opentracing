#!/bin/bash

set -e

[ -z "${SRC_DIR}" ] && export SRC_DIR="`pwd`"
[ -z "${BUILD_DIR}" ] && export BUILD_DIR="`mktemp -d`"

if [[ "$1" == "cmake.test" ]]; then
  cd "${BUILD_DIR}"
  cmake -DCMAKE_BUILD_TYPE=Debug "${SRC_DIR}"
  make
  make test
  exit 0
elif [[ "$1" == "cmake.asan" ]]; then
  cd "${BUILD_DIR}"
  cmake -DCMAKE_BUILD_TYPE=Debug  \
        -DCMAKE_CXX_FLAGS="-fno-omit-frame-pointer -fsanitize=address"  \
        -DCMAKE_SHARED_LINKER_FLAGS="-fno-omit-frame-pointer -fsanitize=address" \
        -DCMAKE_EXE_LINKER_FLAGS="-fno-omit-frame-pointer -fsanitize=address" \
        "${SRC_DIR}"
  make VERBOSE=1
  make test
  exit 0
elif [[ "$1" == "cmake.tsan" ]]; then
  cd "${BUILD_DIR}"
  cmake -DCMAKE_BUILD_TYPE=Debug  \
        -DCMAKE_CXX_FLAGS="-fno-omit-frame-pointer -fsanitize=thread"  \
        -DCMAKE_SHARED_LINKER_FLAGS="-fno-omit-frame-pointer -fsanitize=thread" \
        -DCMAKE_EXE_LINKER_FLAGS="-fno-omit-frame-pointer -fsanitize=thread" \
        "${SRC_DIR}"
  make VERBOSE=1
  make test
  exit 0
elif [[ "$1" == "bazel.build" ]]; then
  cd ci
  bazel build -s --package_path=%workspace%:"${SRC_DIR}" //...
  exit 0
elif [[ "$1" == "cmake.plugin" ]]; then
  "${SRC_DIR}"/ci//build_plugin.sh
  exit 0
elif [[ "$1" == "release" ]]; then
  "${SRC_DIR}"/ci//build_plugin.sh
  "${SRC_DIR}"/ci/release.sh
  exit 0
else
  echo "Invalid do_ci.sh target"
  exit 1
fi
