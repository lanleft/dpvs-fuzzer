# DPVSFuzzer

<!-- introduction -->

## Table of Contents

<!-- toc -->

- [Overview](#overview)
- [Building dependencies](#building-dependencies)
  * [liborotobuf-mutator](#liborotobuf-mutator)
- [Building a Fuzzer](#building-a-fuzzer)

<!-- tocstop -->

## Overview 

- Fuzzer main modules structure:

```bash
  fuzz/
  third_party/
    dpdk-24.11/
    libprotobuf-mutator/
    AFLplusplus/
    dpvs/
```

## Building dependencies

### liborotobuf-mutator

- https://github.com/google/libprotobuf-mutator?tab=readme-ov-file

```bash
# install dependencies
sudo apt-get update
sudo apt-get install protobuf-compiler libprotobuf-dev binutils cmake \
  ninja-build liblzma-dev libz-dev pkg-config autoconf libtool

# compile and test everything
mkdir build
cd build
cmake .. -GNinja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug
ninja check
```

## Building a Fuzzer

```bash
mkdir build 
cd build
cmake -GNinja .. && ninja
```

- Running `dpvs-fuzzer` in the first step:

```bash

export LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/linux-setup/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu

./dpvs-fuzzer
```