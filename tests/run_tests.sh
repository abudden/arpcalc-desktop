#!/bin/bash
# Build tests/unit_tests.cpp (the unit tests ported from the Android
# version) with AddressSanitizer + UBSan and run it.  Needs g++, libmpfr-dev
# and libgmp-dev.
#
# The conversion test that checks results against python-pint is skipped
# unless pint 0.9 can be imported: set PYTHON to a python that has it, e.g.
# PYTHON=~/venv/bin/python tests/run_tests.sh
#
# Usage: tests/run_tests.sh [test name...]  (no names: run every test)

cd "$(dirname "$0")/.." || exit 1

mkdir -p output/tests
g++ -std=c++20 -O1 -g -include cstdint \
	-fsanitize=address,undefined -fno-omit-frame-pointer \
	-I inc tests/unit_tests.cpp \
	src/arpfloat.cpp src/commands.cpp src/conversion.cpp src/grids.cpp \
	src/keys.cpp src/ops.cpp src/si.cpp src/stack.cpp src/strutils.cpp \
	-lmpfr -lgmp -o output/tests/unit_tests || exit 1

ASAN_OPTIONS=detect_leaks=0 output/tests/unit_tests "$@"
