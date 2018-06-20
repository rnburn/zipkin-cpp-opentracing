Name:           zipkin-cpp-opentracing
Summary:        Zipkin library
Version:        1.0
Release:        1
License:        Apache License 2.0
Url:            https://github.com/opentracing/opentracing-cpp
%define _tmppath %(echo $PWD)

%description
Zipkin tracing .so files

%prep

%build
cd %{_tmppath}/

%install
mkdir -p ${RPM_BUILD_ROOT}/usr/lib64/
cp -f %{_tmppath}/zipkin_opentracing/libzipkin_opentracing.so.%{_version_num} $RPM_BUILD_ROOT/usr/lib64/
cp -f %{_tmppath}/zipkin/libzipkin.so.%{_version_num} $RPM_BUILD_ROOT/usr/lib64/

%post
# post-install
# Run ldconfig to generate linker names
/sbin/ldconfig

%postun
# post-uninstall
# Run ldconfig to remove linker names
/sbin/ldconfig


%files
/usr/lib64/libzipkin_opentracing.so.%{_version_num}
/usr/lib64/libzipkin.so.%{_version_num}
