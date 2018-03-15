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
cd "${BUILD_DIR}"
git clone https://github.com/opentracing/opentracing-cpp.git
cd opentracing-cpp
mkdir .build && cd .build
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-fPIC" \
      -DCMAKE_INSTALL_PREFIX="${BUILD_DIR}" \
      -DBUILD_SHARED_LIBS=OFF \
      -DBUILD_TESTING=OFF \
      -DBUILD_MOCKTRACER=OFF \
      ..
make && make install

# Build zipkin
cd "${BUILD_DIR}"
mkdir zipkin-cpp-opentracing && cd zipkin-cpp-opentracing
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-fPIC" \
      -DCMAKE_INSTALL_PREFIX="${BUILD_DIR}" \
      -DBUILD_SHARED_LIBS=OFF \
      -DBUILD_TESTING=OFF \
      "${SRC_DIR}"
make && make install

# Create a plugin
cd "${BUILD_DIR}"
mkdir zipkin-opentracing-plugin && cd zipkin-opentracing-plugin
cat <<EOF > Makefile
all:
	gcc -shared -o libzipkin_opentracing_plugin.so \
			-L${BUILD_DIR}/lib \
			-Wl,--whole-archive \
			${BUILD_DIR}/lib/libzipkin_opentracing.a \
			-Wl,--no-whole-archive \
			${BUILD_DIR}/lib/libzipkin.a 
EOF
make
cp libzipkin_opentracing_plugin.so /
