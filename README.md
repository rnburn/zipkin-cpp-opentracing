# zipkin-cpp-opentracing
OpenTracing implementation for Zipkin in C++.

## Dependencies

* The [C++ OpenTracing Library](https://github.com/opentracing/opentracing-cpp).
* [curl](https://curl.haxx.se/)

## Compile and install
```
mkdir .build
cd .build
cmake ..
make
sudo make install
```

## Examples

See [tutorial.cc](zipkin_opentracing/example/tutorial.cc).
