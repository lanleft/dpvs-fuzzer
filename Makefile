
buildfuzz:
	echo "Building dpvs-fuzz"
	cd build &&  cmake -GNinja .. && ninja && cd ..

# AFL_SKIP_CPUFREQ=1 AFL_QEMU_PERSISTENT_GPR=1 ../third_party/AFLplusplus/afl-fuzz -i ../input-seeds  -o ../output-fuzz -- ./dpvs-fuzzer
runafl:
	sudo rm -rf output-fuzz
	sudo LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/linux-setup/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 AFL_SKIP_CPUFREQ=1 AFL_QEMU_PERSISTENT_GPR=1 third_party/AFLplusplus/afl-fuzz -i input-seeds  -o output-fuzz -- ./build/dpvs-fuzzer @@

# run test case with input filename
testone:
	sudo  LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/linux-setup/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 ./build/dpvs-fuzzer ${fn}

gdbtest:
	sudo LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/linux-setup/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 LC_CTYPE=C.UTF-8 gdb --args ./build/dpvs-fuzzer ${fn}