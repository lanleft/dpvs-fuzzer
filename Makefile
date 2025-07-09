
# Define paths for llvm tools
LLVM_TOOLS_DIR=/usr/lib/llvm-18/bin
LLVM_PROFDATA=$(LLVM_TOOLS_DIR)/llvm-profdata
LLVM_COV=$(LLVM_TOOLS_DIR)/llvm-cov

buildfuzz:
	echo "Building dpvs-fuzz"
	cd build &&  cmake -GNinja .. && ninja && cd ..

runafl:
	output_dir=${od}; \
	input_dir=${id}; \
	LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/dpvs-project/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 AFL_SKIP_CPUFREQ=1 AFL_QEMU_PERSISTENT_GPR=1 AFL_PERSISTENT_RECORD=10 __AFL_PERSISTENT=1 AFL_AUTORESUME=1 third_party/AFLplusplus/afl-fuzz -i $$input_dir  -o $$output_dir -- ./build/dpvs-fuzzer @@

dbgafl:
	output_dir=${od}; \
	input_dir=${id}; \
	rm -rf $$output_dir; \
	LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/dpvs-project/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 AFL_SKIP_CPUFREQ=1 AFL_QEMU_PERSISTENT_GPR=1 AFL_PERSISTENT_RECORD=10 __AFL_PERSISTENT=1 AFL_LLVM_LAF_ALL=1 AFL_DEBUG=1 third_party/AFLplusplus/afl-fuzz -i $$input_dir  -o $$output_dir -- ./build/dpvs-fuzzer @@

testone:
	@echo "Looking for test case with id=${id}"; \
	test_id="id:00000${id}"; \
	fn=$$(ls output-fuzz/default/crashes/ | grep "$$test_id" | head -n1); \
	if [ -z "$$fn" ]; then \
		echo "No test case found for id: $$test_id"; \
		exit 1; \
	fi; \
	echo "Running test case: $$fn"; \
	cat "output-fuzz/default/crashes/$$fn"; \
	LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/dpvs-project/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu \
	ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 \
	./build/dpvs-fuzzer "output-fuzz/default/crashes/$$fn"

testrd:
	# test random test case of folder ${folder}
	# get first file of output-fuzz/default/queue
	folder=${folder}; \
	fn=$$(ls $(folder)/default/queue/ | head -n1); \
	if [ -z "$$fn" ]; then \
		echo "No test case found in $(folder)/default/queue"; \
		exit 1; \
	fi; \
	echo "Running test case: $$fn"; \
	cat "$(folder)/default/queue/$$fn"; \
	LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/dpvs-project/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu \
	ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 \
	./build/dpvs-fuzzer "$(folder)/default/queue/$$fn"
	
testN:
	# testing last N test cases of folder ${folder}
	folder=${folder}; \
	N=${N}; \
	fn=$$(ls $(folder)/default/queue/ | head -n$$N); \
	for file in $$fn; do \
		echo "Running test case: $(folder)/default/queue/$$file"; \
		LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/dpvs-project/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu \
		ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 \
		./build/dpvs-fuzzer "$(folder)/default/queue/$$file"; \
	done

testseed:
	LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/dpvs-project/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu \
	ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 \
	./build/dpvs-fuzzer input-seeds/seed0

testip:
	LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/dpvs-project/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu \
	ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 \
	./build/dpvs-fuzzer ${ip}

batchtest:
	@echo "Looking for test cases starting with id=${id}"; \
	test_id="*:00000${id},"; \
	output_file="output-fuzz/lists.txt"; \
	find output-fuzz/default/crashes/ -type f -name "$$test_id*" | sort > "$$output_file"; \
	if [ ! -s "$$output_file" ]; then \
		echo "No matching test cases found for id: $$test_id"; \
		exit 1; \
	fi; \
	echo "Running batch of test cases"; \
	LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/dpvs-project/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu \
	ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 \
	FUZZ_BATCH_TEST=output-fuzz/lists.txt \
	./build/dpvs-fuzzer-batch

batchgdb:
	@echo "Looking for test cases starting with id=${id}"; \
	test_id="*:00000${id},"; \
	output_file="output-fuzz/lists.txt"; \
	find output-fuzz/default/crashes/ -type f -name "$$test_id*" | sort > "$$output_file"; \
	if [ ! -s "$$output_file" ]; then \
		echo "No matching test cases found for id: $$test_id"; \
		exit 1; \
	fi; \
	echo "Running batch of test cases"; \
	LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/dpvs-project/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu \
	ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 \
	FUZZ_BATCH_TEST=output-fuzz/lists.txt \
	LC_CTYPE=C.UTF-8 gdb ./build/dpvs-fuzzer-batch

# run test case under gdb
gdbtest:
	@echo "Looking for test case with id=${id}"; \
	test_id="id:00000${id}"; \
	fn=$$(ls output-fuzz/default/crashes/ | grep "$$test_id" | head -n1); \
	if [ -z "$$fn" ]; then \
		echo "No test case found for id: $$test_id"; \
		exit 1; \
	fi; \
	echo "Running GDB with test case: $$fn"; \
	LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/dpvs-project/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu \
	ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 \
	LC_CTYPE=C.UTF-8 \
	gdb --args ./build/dpvs-fuzzer "output-fuzz/default/crashes/$$fn"

gdbrd:
	# test random test case of folder ${folder}
	# get first file of output-fuzz/default/queue
	folder=${folder}; \
	fn=$$(ls $(folder)/default/queue/ | head -n1); \
	if [ -z "$$fn" ]; then \
		echo "No test case found in $(folder)/default/queue"; \
		exit 1; \
	fi; \
	echo "Running test case: $$fn"; \
	cat "$(folder)/default/queue/$$fn"; \
	LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/dpvs-project/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu \
	ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 \
	LC_CTYPE=C.UTF-8 \
	gdb --args ./build/dpvs-fuzzer "$(folder)/default/queue/$$fn"

gdbip:
	# gdb test case with input filename ${ip}
	LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/dpvs-project/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu \
	ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 \
	LC_CTYPE=C.UTF-8 \
	gdb --args ./build/dpvs-fuzzer ${ip}

# sudo make gencov folder=output-fuzz op=cov_1405
gencov:
ifndef folder
	$(error folder variable is required, usage: make gencov folder=your_folder_path)
endif
	@echo "Generating coverage report for folder=${folder}"

	# Create new variable with /default/queue appended
	$(eval queue_folder=${folder}/default/queue)

	# Get cov_report from ${op}
	$(eval cov_report=${op})

	# Step 1: Clean and prepare
	rm -rf $(cov_report)
	mkdir -p $(cov_report)

	
	# Step 2: Set LLVM_PROFILE_FILE env (correct project name)
	export LLVM_PROFILE_FILE="$(cov_report)/dpvs-fuzzer.%4m.%p.profraw"

	# Step 3: Run tests **one by one sequentially**
	@cd $(cov_report) && \
	total=$$(ls ../$(queue_folder) | wc -l); \
	i=1; \
	for file in ../$(queue_folder)/*; do \
		if [ -f "$$file" ]; then \
			echo "[$$i/$$total] Running coverage for $$file"; \
			basefile=$$(basename $$file); \
			sudo LLVM_PROFILE_FILE="dpvs-fuzzer-$$basefile.profraw" \
			LD_LIBRARY_PATH=/home/lab/Desktop/side-projects/dpvs-project/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu \
			ASAN_OPTIONS=detect_leaks=0,abort_on_error=1,symbolize=0 \
			../build/dpvs-fuzzer-cov "$$file"; \
			i=$$((i+1)); \
		fi; \
	done

	# Step 4: Merge .profraw files
	find $(cov_report) -name '*.profraw' -print0 | xargs -0 $(LLVM_PROFDATA) merge -o $(cov_report)/coverage.profdata --num-threads=$$(nproc)

	# Step 5: Generate HTML coverage report from object files
	@echo "Generating HTML coverage report from object files..."
	@mkdir -p $(cov_report)/html
	@$(LLVM_COV) show \
		-format=html \
		-output-dir=$(cov_report)/html \
		-instr-profile=$(cov_report)/coverage.profdata \
		-object /home/lab/Desktop/side-projects/dpvs-project/dpvs-fuzzer/build/dpvs-fuzzer-cov \
		-compilation-dir=/home/lab/Desktop/side-projects/dpvs-project/dpvs-fuzzer/build/CMakeFiles/dpvs-fuzzer-cov.dir/
		

catfiles:
	# cat all files in folder ${folder}
	$(eval cat_folder=${folder}/default/queue)
	# ls $(cat_folder)/* | xargs cat
	# for loop to print filename and file content
	for file in $(cat_folder)/*; do \
		echo "$$file"; \
		cat "$$file"; \
		echo "\n--------------------------------\n"; \
	done


genpb:
	protoc --python_out=./protocOut --proto_path=../../fuzz/  mbuf_mutator.proto

pb2txt:
	@base_name=$$(echo ${ip} | cut -d ',' -f 1 | awk -F'/' '{print $$NF}'); \
	echo $$base_name; \
	output_file=scripts/seed/text-seeds/$$base_name; \
	python3 scripts/seed/pb2txt.py $(ip) $$output_file; \
	cat $$output_file


txt2pb:
	cd scripts/seed && protoc --python_out=./protocOut --proto_path=../../fuzz/  mbuf_mutator.proto && cd ../.. 
	python3 scripts/seed/txt2pb.py $(ip) $(op)

# output file is scripts/seed/text-seeds/ + basename of input file with the first part of ','
# example id:000000,sig:06,src:000000,time:775,execs:721,op:flip1,pos:16  ->  id:000000
# get last part of '/
sendpb:
	@base_name=$$(echo ${ip} | cut -d ',' -f 1 | awk -F'/' '{print $$NF}'); \
	echo $$base_name; \
	output_file=scripts/seed/text-seeds/$$base_name; \
	python3 scripts/seed/pb2txt.py ${ip} $$output_file; \
	cat $$output_file; \
	python3 scripts/seed/send-txtproto.py $$output_file