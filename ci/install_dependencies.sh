#!/bin/bash

set -e
apt-get update 
apt-get install --no-install-recommends --no-install-suggests -y \
                libcurl4-openssl-dev \
                build-essential \
                cmake \
                git \
                ca-certificates

cd /
git clone https://github.com/opentracing/opentracing-cpp.git
cd opentracing-cpp
mkdir .build && cd .build
cmake -DBUILD_TESTING=OFF ..
make && make install
