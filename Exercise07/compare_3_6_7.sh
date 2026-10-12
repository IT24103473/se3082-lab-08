#!/bin/bash
# Times Exercise 3 (MPI_Send + fixed source), Exercise 6 (MPI_ANY_SOURCE) and
# Exercise 7 (MPI_Bsend + MPI_ANY_SOURCE) the same way, so they can be compared fairly.
# The programs are run in turn (3, 6, 7, 3, 6, 7, ...) so that any slow-down of the
# laptop affects all three equally. Usage (from the repo root, after "make"):
#   ./Exercise07/compare_3_6_7.sh
cd "$(dirname "$0")/.."

REPS=7
OUT=Exercise07/comparison.csv
if [ "$(id -u)" = 0 ]; then
    export OMPI_ALLOW_RUN_AS_ROOT=1 OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1
fi

echo "program,processes,run,time_sec,pi,receive_order" > $OUT
for p in 4 8; do
    for r in $(seq 1 $REPS); do
        for prog in pi_mpi pi_any_source pi_bsend; do
            o=$(mpirun --use-hwthread-cpus -np $p ./$prog)
            t=$(echo "$o" | grep "^Time" | awk '{print $(NF-1)}')
            pi=$(echo "$o" | grep "^Estimate" | awk '{print $NF}')
            ord=$(echo "$o" | grep "^Receive order" | cut -d= -f2 | xargs)
            echo "$prog,$p,$r,$t,$pi,$ord" >> $OUT
        done
    done
done
echo "saved $OUT"
