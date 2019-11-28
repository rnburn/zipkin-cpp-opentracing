#!/bin/bash

set -e
apt-get update
apt-get install --no-install-recommends --no-install-suggests -y \
                libcurl4-openssl-dev \
                build-essential \
                cmake \
                git \
                ca-certificates

# Build OpenTracing
cd /
export OPENTRACING_VERSION=1.5.0
git clone -b v$OPENTRACING_VERSION https://github.com/opentracing/opentracing-cpp.git
cd opentracing-cpp
mkdir .build && cd .build
cmake -DBUILD_TESTING=OFF ..
make && make install
