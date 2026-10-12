# Lab 8 (MPI) - build every program with:  make
# No -O flag on purpose: the lab compiles with plain "mpicc -o program program.c",
# and every timing in Exercise 4 was measured with this build.
CC  = mpicc
CXX = mpic++
CFLAGS = -Wall

PROGRAMS = job1 messages1 message2 sum_mpi monte_pi pi_mpi \
           messages_mismatch message2_bsend pi_any_source pi_bsend

all: $(PROGRAMS)

# Exercise 0 - in-class activities (the course files are C++, so they go through mpic++)
job1: Exercise00/HelloMPI.c
	$(CXX) $(CFLAGS) -x c++ -o $@ $<
messages1: Exercise00/messages1.cc
	$(CXX) $(CFLAGS) -o $@ $<
message2: Exercise00/messaage2.cc
	$(CXX) $(CFLAGS) -o $@ $<

# Exercise 2 - sum 1..10,000,000
sum_mpi: Exercise02/sum_mpi.c
	$(CC) $(CFLAGS) -o $@ $<

# Exercise 3 - Monte Carlo Pi (serial sample + MPI version)
monte_pi: Exercise03/monte_pi.c
	gcc $(CFLAGS) -o $@ $<
pi_mpi: Exercise03/pi_mpi.c
	$(CC) $(CFLAGS) -o $@ $< -lm

# Exercise 5 - mismatched source/destination, and Buffered Send
messages_mismatch: Exercise05/messages_mismatch.cc
	$(CXX) $(CFLAGS) -o $@ $<
message2_bsend: Exercise05/message2_bsend.cc
	$(CXX) $(CFLAGS) -o $@ $<

# Exercise 6 - receive from MPI_ANY_SOURCE
pi_any_source: Exercise06/pi_any_source.c
	$(CC) $(CFLAGS) -o $@ $< -lm

# Exercise 7 - Exercise 6 with MPI_Bsend
pi_bsend: Exercise07/pi_bsend.c
	$(CC) $(CFLAGS) -o $@ $< -lm

clean:
	rm -f $(PROGRAMS)

.PHONY: all clean
