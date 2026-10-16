#!/bin/bash
# Build tests/unit_tests.cpp (the unit tests ported from the Android
# version) and tests/hms_tests.cpp (H.MMSS arithmetic) with AddressSanitizer
# + UBSan and run them.  Needs g++, libmpfr-dev and libgmp-dev.
#
# The conversion test that checks results against python-pint is skipped
# unless pint can be imported: set PYTHON to a python that has it, e.g.
# PYTHON=~/venv/bin/python tests/run_tests.sh
#
# Usage: tests/run_tests.sh [test name...]  (no names: run every test)
# Set HMS_CASES for more random H.MMSS cases (default 2000 of each kind).

cd "$(dirname "$0")/.." || exit 1

mkdir -p output/tests
g++ -std=c++20 -O1 -g -include cstdint \
	-fsanitize=address,undefined -fno-omit-frame-pointer \
	-I inc tests/unit_tests.cpp \
	src/arpfloat.cpp src/commands.cpp src/conversion.cpp src/grids.cpp \
	src/keys.cpp src/ops.cpp src/si.cpp src/stack.cpp src/strutils.cpp \
	-lmpfr -lgmp -o output/tests/unit_tests || exit 1

g++ -std=c++20 -O1 -g -include cstdint \
	-fsanitize=address,undefined -fno-omit-frame-pointer \
	-I inc tests/hms_tests.cpp \
	src/arpfloat.cpp src/commands.cpp src/conversion.cpp src/grids.cpp \
	src/keys.cpp src/ops.cpp src/si.cpp src/stack.cpp src/strutils.cpp \
	-lmpfr -lgmp -o output/tests/hms_tests || exit 1

ASAN_OPTIONS=detect_leaks=0 output/tests/unit_tests "$@"
result=$?

# Arithmetic in H.MMSS, D.MMSS and D.MM (HMS_CASES random cases of each)
echo
ASAN_OPTIONS=detect_leaks=0 output/tests/hms_tests ${HMS_CASES:-2000} || result=1
exit $result
