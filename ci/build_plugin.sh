#!/bin/bash

set -e
apt-get update 
apt-get install --no-install-recommends --no-install-suggests -y \
                build-essential \
                cmake \
                wget \
                git \
                ca-certificates
# Install libcurl
CURL_VERSION=7.59.0
cd "${BUILD_DIR}"
wget https://curl.haxx.se/download/curl-${CURL_VERSION}.tar.gz
tar zxf curl-${CURL_VERSION}.tar.gz
cd curl-${CURL_VERSION}
./configure --prefix="${BUILD_DIR}" \
            --disable-ftp \
            --disable-ldap \
            --disable-dict \
            --disable-telnet \
            --disable-tftp \
            --disable-pop3 \
            --disable-smtp \
            --disable-gopher \
            --without-ssl \
            --disable-crypto-auth \
            --without-axtls \
            --disable-rtsp \
            --enable-shared=no \
            --enable-static=yes \
            --with-pic
make && make install

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
cat <<EOF > export.map
{
  global:
    OpenTracingMakeTracerFactory;
  local: *;
};
EOF
cat <<EOF > Makefile
all:
	gcc -shared -o libzipkin_opentracing_plugin.so \
      -Wl,--version-script=export.map \
			-L${BUILD_DIR}/lib \
			-Wl,--whole-archive \
			${BUILD_DIR}/lib/libzipkin_opentracing.a \
			-Wl,--no-whole-archive \
			${BUILD_DIR}/lib/libzipkin.a \
			${BUILD_DIR}/lib/libcurl.a \
      -static-libstdc++ -static-libgcc
EOF
make
cp libzipkin_opentracing_plugin.so /
