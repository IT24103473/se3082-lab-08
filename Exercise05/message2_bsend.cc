// Exercise 5.2: messaage2.cc rewritten with Buffered Send (MPI_Bsend) instead of MPI_Ssend.
// The variables are unchanged: rank 1 still sends the array x, rank 3 still receives into y.
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
    int x[10], y[10];
    if (rank == 1) {
      for (int r =0;r <10; r++)
         x[r] = 10*r;

      // Buffered send needs a buffer that we attach ourselves:
      // the packed size of 10 ints plus MPI_BSEND_OVERHEAD bytes.
      int packed, bufsize;
      MPI_Pack_size(10, MPI_INT, MPI_COMM_WORLD, &packed);
      bufsize = packed + MPI_BSEND_OVERHEAD;
      char *buffer = (char *)malloc(bufsize);
      MPI_Buffer_attach(buffer, bufsize);

      printf("SEnding message to computer 3 from computer 1\n");
      MPI_Bsend(x, 10, MPI_INT, 3, 0, MPI_COMM_WORLD);   // CHANGED: MPI_Ssend -> MPI_Bsend
      printf("computer 1: MPI_Bsend returned (x was copied into the attached buffer)\n");

      // Detach blocks until the buffered message has been delivered; then the buffer can be freed
      MPI_Buffer_detach(&buffer, &bufsize);
      free(buffer);
    }
    else if (rank == 3) {
      MPI_Recv(y, 10, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
      printf("in computer 3 the value of y is printed\n");
      for (int r=0;r<10;r++)
         printf(" %d ",y[r]);
      printf("\n");
    }
    else
      printf("Just a normal process From rank %d machine %s\n", rank, name);
    MPI_Finalize();
}
