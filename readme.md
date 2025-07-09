# DPVSFuzzer

<!-- introduction -->

## Table of Contents

<!-- toc -->

- [Set up Debugging Environment](#set-up-debugging-environment)
- [Fuzzer Dependencies](#fuzzer-dependencies)
  * [Project Structure](#project-structure)
  * [Building Dependencies](#building-dependencies)
    + [liborotobuf-mutator](#liborotobuf-mutator)
  * [Building Fuzzer](#building-fuzzer)
  * [Testing Command](#testing-command)
- [References](#references)

<!-- tocstop -->

## Set up Debugging Environment

- To install dpvs, we come up with several ideas. In the end, there's one solution work out. 
- Basically, we create 2 qemu machine. The first one is for setting dpvs with dpdk instrumentation, that requires huge storeage, ram and cores. The second machine is quite simple, cause its purpose is just testing connection. 
- More details and step by step guideline [here](vm-setup/readme.md)

## Fuzzer Dependencies

### Project Structure
- Fuzzer main modules structure:

```bash
  fuzz/
  third_party/
    dpdk-24.11/
    libprotobuf-mutator/
    AFLplusplus/
    dpvs/
```

### Building Dependencies

#### liborotobuf-mutator

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

### Building Fuzzer

```bash
# 
make buildfuzz
```

- Running `dpvs-fuzzer` in the first step:

### Testing Command

```bash
# or using make command
sudo make runafl od=output-fuzz id=scripts/seed/binary-seeds
```

## References

- https://chromium.googlesource.com/chromium/src/+/main/testing/libfuzzer/libprotobuf-mutator.md
- https://github.com/github/securitylab/tree/main/SecurityExploits/apple/darwin-xnu/icmp_error_CVE-2018-4407
- https://github.com/iqiyi/dpvs/commit/e9fdedfa40a0a624e8c67ab4625dcc097b3896c9
- https://github.com/googleprojectzero/SockFuzzer
