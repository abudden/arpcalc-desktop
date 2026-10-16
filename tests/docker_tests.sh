#!/bin/bash
# Run in the cpparpcalc_tests docker container (see docker-compose.yml): the
# unit tests, the H.MMSS tests and a short random key-press run.
cd "$(dirname "$0")/.." || exit 1

result=0
bash tests/run_tests.sh || result=1
echo
bash tests/run_fuzz.sh 1 ${FUZZ_SEEDS:-20} || result=1
exit $result
