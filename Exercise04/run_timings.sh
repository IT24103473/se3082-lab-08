#!/bin/bash
# Exercise 4: time Exercise 2 (sum_mpi) and Exercise 3 (pi_mpi) for 1..12 processes.
# Each case is run REPS times; every run is saved to timings.csv (the plots use the median).
# Usage (from the repo root, after "make"):  ./Exercise04/run_timings.sh
cd "$(dirname "$0")/.."

REPS=7
OUT=Exercise04/timings.csv
if [ "$(id -u)" = 0 ]; then
    export OMPI_ALLOW_RUN_AS_ROOT=1 OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1
fi

echo "program,processes,run,time_sec" > $OUT
for prog in sum_mpi pi_mpi; do
    for p in 1 2 3 4 5 6 7 8 9 10 11 12; do
        for r in $(seq 1 $REPS); do
            t=$(mpirun --use-hwthread-cpus -np $p ./$prog | grep -E "^Time" | awk '{print $(NF-1)}')
            echo "$prog,$p,$r,$t" >> $OUT
        done
        echo "$prog  np=$p  done"
    done
done
