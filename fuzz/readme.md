# Designing a DPVS fuzzer 

## Table of Content


## Building libprotobuf-mutator

```bash
debugpc at ~/Desktop/side-projects/linux-setup/dpvs-fuzzer/libprotobuf-mutator/build ❯ sudo ninja install
[0/1] Install the project...
-- Install configuration: "Debug"
-- Installing: /usr/local/lib/libprotobuf-mutator-libfuzzer.a
-- Installing: /usr/local/lib/libprotobuf-mutator.a
-- Installing: /usr/local/lib/cmake/libprotobuf-mutator/libprotobuf-mutatorTargets.cmake
-- Installing: /usr/local/lib/cmake/libprotobuf-mutator/libprotobuf-mutatorTargets-debug.cmake
-- Installing: /usr/local/lib/cmake/libprotobuf-mutator/libprotobuf-mutatorConfig.cmake
-- Installing: /usr/local/share/pkgconfig/libprotobuf-mutator.pc
-- Installing: /usr/local/include/libprotobuf-mutator/port
-- Installing: /usr/local/include/libprotobuf-mutator/port/gtest.h
-- Installing: /usr/local/include/libprotobuf-mutator/port/protobuf.h
-- Installing: /usr/local/include/libprotobuf-mutator/src
-- Installing: /usr/local/include/libprotobuf-mutator/src/utf8_fix.h
-- Installing: /usr/local/include/libprotobuf-mutator/src/binary_format.h
-- Installing: /usr/local/include/libprotobuf-mutator/src/text_format.h
-- Installing: /usr/local/include/libprotobuf-mutator/src/random.h
-- Installing: /usr/local/include/libprotobuf-mutator/src/field_instance.h
-- Installing: /usr/local/include/libprotobuf-mutator/src/weighted_reservoir_sampler.h
-- Installing: /usr/local/include/libprotobuf-mutator/src/mutator.h
-- Installing: /usr/local/include/libprotobuf-mutator/src/libfuzzer
-- Installing: /usr/local/include/libprotobuf-mutator/src/libfuzzer/libfuzzer_mutator.h
-- Installing: /usr/local/include/libprotobuf-mutator/src/libfuzzer/libfuzzer_macro.h

```

## Building DPDK


## High-level DPVS fuzzing Architecture


## Building fuzzer


## ...

## Testing 