#!/bin/bash
# Build tests/fuzz_core.cpp with AddressSanitizer + UBSan and run it over a
# range of seeds.  Needs g++, libmpfr-dev and libgmp-dev.
#
# Usage: tests/run_fuzz.sh [first_seed] [last_seed] [steps]
# A failing seed can be replayed with: TRACE=1 output/fuzz/fuzz_core <seed> <steps>

cd "$(dirname "$0")/.." || exit 1

first=${1:-1}
last=${2:-100}
steps=${3:-4000}

mkdir -p output/fuzz
g++ -std=c++20 -O1 -g -include cstdint \
	-fsanitize=address,undefined -fno-omit-frame-pointer \
	-I inc tests/fuzz_core.cpp \
	src/arpfloat.cpp src/commands.cpp src/conversion.cpp src/grids.cpp \
	src/keys.cpp src/ops.cpp src/si.cpp src/stack.cpp src/strutils.cpp \
	-lmpfr -lgmp -o output/fuzz/fuzz_core || exit 1

failures=0
for seed in $(seq "$first" "$last"); do
	# A timeout catches hangs (e.g. a loop that never terminates)
	ASAN_OPTIONS=detect_leaks=0 timeout 60 output/fuzz/fuzz_core "$seed" "$steps" \
		> /dev/null 2> "output/fuzz/seed$seed.log"
	result=$?
	if [ $result -ne 0 ]; then
		failures=$((failures + 1))
		echo "Seed $seed failed (exit code $result): see output/fuzz/seed$seed.log"
	else
		rm -f "output/fuzz/seed$seed.log"
	fi
done

echo "$failures failure(s) in seeds $first to $last"
[ $failures -eq 0 ]
