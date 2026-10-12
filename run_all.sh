#!/bin/bash
# Runs every Lab 8 program and saves the terminal output into each exercise folder
# (ExerciseXX/output.txt). Usage:  make && ./run_all.sh
cd "$(dirname "$0")"

MPIRUN="mpirun --use-hwthread-cpus"

# WSL Ubuntu here only has the root account, and Open MPI refuses to start as root
# unless these two variables are set. A normal user account does not need them.
if [ "$(id -u)" = 0 ]; then
    export OMPI_ALLOW_RUN_AS_ROOT=1 OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1
fi

# run <output file> <command...> : prints the command like a terminal, then its output
run() {
    local out=$1; shift
    { echo "\$ $*"; "$@" 2>&1; echo; } | tee -a "$out"
}

# ---- Exercise 0: in-class activities from SE4060-MPI-Part1
f=Exercise00/output.txt; : > $f
run $f $MPIRUN -np 4 ./job1
run $f $MPIRUN -np 4 ./messages1
run $f $MPIRUN -np 4 ./message2

# ---- Exercise 2: sum 1..10,000,000
f=Exercise02/output.txt; : > $f
for p in 1 2 4 8; do run $f $MPIRUN -np $p ./sum_mpi; done

# ---- Exercise 3: Monte Carlo Pi, 10,000,000 points
f=Exercise03/output.txt; : > $f
{ echo "\$ echo 10000000 | ./monte_pi      (serial sample program)"; echo 10000000 | ./monte_pi; echo; echo; } | tee -a $f
for p in 1 2 4 8; do run $f $MPIRUN -np $p ./pi_mpi; done

# ---- Exercise 5.1: source and destination do not match (hangs, so stop it after 10 s)
f=Exercise05/output_mismatch.txt; : > $f
{ echo "\$ timeout 10 $MPIRUN -np 4 ./messages_mismatch"
  start=$(date +%s)
  timeout 10 $MPIRUN -np 4 ./messages_mismatch 2>&1
  code=$?
  if [ $code = 124 ]; then
      echo "[program was still waiting after $(( $(date +%s) - start )) s and was stopped by timeout]"
  else
      echo "[program ended by itself with exit code $code]"
  fi
  echo; } | tee -a $f

# ---- Exercise 5.2: message2 with MPI_Bsend
f=Exercise05/output_bsend.txt; : > $f
run $f $MPIRUN -np 4 ./message2_bsend

# ---- Exercise 6 and 7: compare with Exercise 3 (same process counts)
for ex in "Exercise06 pi_any_source" "Exercise07 pi_bsend"; do
    set -- $ex
    f=$1/output.txt; : > $f
    for p in 1 2 4 8; do run $f $MPIRUN -np $p ./$2; done
done
