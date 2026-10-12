// Exercise 5.1: messages1.cc with the source and destination NOT matching.
// Rank 1 still sends to rank 3, but rank 3 now waits for a message from rank 2,
// and rank 2 never sends anything.
#include <cstdio>
#include <cstdlib>
#include <mpi.h>
int main(void)
{
    int rank;
    MPI_Status status;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    char name[30];
    int len;
    MPI_Get_processor_name( name, &len );
    int x, y;
    x = 30;
    if (rank == 1) {
      printf("SEnding message to computer 3 from computer 1\n");
      fflush(stdout);
      MPI_Ssend(&x, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
      printf("computer 1: send finished\n");          // never printed
    }
    else if (rank == 3) {
      printf("computer 3: waiting for a message from computer 2\n");
      fflush(stdout);
      MPI_Recv(&y, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);   // CHANGED: source 1 -> 2
      printf("in computer 3 the value of y is %d\n", y);         // never printed
    }
    else
      printf("Just a normal process From rank %d machine %s\n", rank, name);
    fflush(stdout);
    MPI_Finalize();
}
