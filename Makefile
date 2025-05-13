
buildfuzz:
	echo "Building dpvs-fuzz"
	cd build &&  cmake -GNinja .. && ninja && cd ..

# AFL_SKIP_CPUFREQ=1 AFL_QEMU_PERSISTENT_GPR=1 ../third_party/AFLplusplus/afl-fuzz -i ../input-seeds  -o ../output-fuzz -- ./dpvs-fuzzer
runafl:
	ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 AFL_SKIP_CPUFREQ=1 AFL_QEMU_PERSISTENT_GPR=1 third_party/AFLplusplus/afl-fuzz -i input-seeds  -o output-fuzz -- ./build/dpvs-fuzzer @@

# run test case with input filename
testone:
	ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 ./build/dpvs-fuzzer ${fn}