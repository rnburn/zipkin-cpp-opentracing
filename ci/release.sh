#!/bin/bash

set -e
apt-get update 
apt-get install --no-install-recommends --no-install-suggests -y \
                wget \
                unzip

# Install ghr
cd /
wget https://github.com/tcnksm/ghr/releases/download/v0.5.4/ghr_v0.5.4_linux_amd64.zip
unzip ghr_v0.5.4_linux_amd64.zip

# Create packaged plugins
gzip -c /libzipkin_opentracing_plugin.so > linux-amd64-libzipkin_opentracing_plugin.so.gz

# Create release
cd "${BUILD_DIR}"
/ghr -t $GITHUB_TOKEN \
     -u $CIRCLE_PROJECT_USERNAME \
     -r $CIRCLE_PROJECT_REPONAME \
     -replace \
     `git describe --tags` \
     /linux-amd64-libzipkin_opentracing_plugin.so.gz
