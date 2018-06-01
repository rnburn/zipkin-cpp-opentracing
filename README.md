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

## Dynamic loading

The Zipkin tracer supports dynamic loading and construction from a JSON configuration. See the [schema](zipkin_opentracing/tracer_configuration.schema.json) for details on the JSON format.
