Name:           zipkin-devel
Summary:        Zipkin headers
Version:        1.0
Release:        1
License:        Apache License 2.0
URL:            https://github.com/rnburn/zipkin

%description
header files for zipkin

%prep

%build

%install
mkdir -p ${RPM_BUILD_ROOT}/usr/include/zipkin
cp -f %{_origdir}/zipkin/include/zipkin/hex.h $RPM_BUILD_ROOT/usr/include/zipkin
cp -f %{_origdir}/zipkin/include/zipkin/optional.h $RPM_BUILD_ROOT/usr/include/zipkin
cp -f %{_origdir}/zipkin/include/zipkin/trace_id.h $RPM_BUILD_ROOT/usr/include/zipkin

cp -f %{_origdir}/zipkin/include/zipkin/tracer.h $RPM_BUILD_ROOT/usr/include/zipkin
cp -f %{_origdir}/zipkin/include/zipkin/utility.h $RPM_BUILD_ROOT/usr/include/zipkin
cp -f %{_origdir}/zipkin/include/zipkin/span_context.h $RPM_BUILD_ROOT/usr/include/zipkin
cp -f %{_origdir}/zipkin/include/zipkin/tracer_interface.h $RPM_BUILD_ROOT/usr/include/zipkin
cp -f %{_origdir}/zipkin/include/zipkin/zipkin_core_types.h $RPM_BUILD_ROOT/usr/include/zipkin
cp -f %{_origdir}/zipkin/include/zipkin/ip_address.h $RPM_BUILD_ROOT/usr/include/zipkin
cp -f %{_origdir}/zipkin/include/zipkin/flags.h $RPM_BUILD_ROOT/usr/include/zipkin

cp -f %{_origdir}/zipkin_opentracing/include/zipkin/opentracing.h $RPM_BUILD_ROOT/usr/include/zipkin


%files
/usr/include/zipkin/hex.h
/usr/include/zipkin/optional.h
/usr/include/zipkin/trace_id.h
/usr/include/zipkin/tracer.h
/usr/include/zipkin/utility.h
/usr/include/zipkin/span_context.h
/usr/include/zipkin/tracer_interface.h
/usr/include/zipkin/zipkin_core_types.h
/usr/include/zipkin/ip_address.h
/usr/include/zipkin/flags.h
/usr/include/zipkin/opentracing.h
