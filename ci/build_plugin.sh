#!/bin/bash

set -e
apt-get update 
apt-get install --no-install-recommends --no-install-suggests -y \
                build-essential \
                cmake \
                git \
                ca-certificates

# Build OpenTracing
cd "${BUILD_DIR}"
git clone https://github.com/opentracing/opentracing-cpp.git
cd opentracing-cpp
mkdir .build && cd .build
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-fPIC" \
      -DCMAKE_INSTALL_REFIX="${BUILD_DIR}" \
      -DBUILD_SHARED_LIBS=OFF \
      -DBUILD_TESTING=OFF \
      -DBUILD_MOCKTRACER=OFF \
      ..
make && make install
