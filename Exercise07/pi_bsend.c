/* Exercise 7: Exercise 6 (Monte Carlo Pi, receive from MPI_ANY_SOURCE) rewritten so the
   workers send with Buffered Send (MPI_Bsend) instead of MPI_Send. */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SEED  35791246
#define NITER 10000000LL            /* total number of random points (all processes) */

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long my_niter = NITER / size + (rank < NITER % size ? 1 : 0);
    srand(SEED + rank);

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    long long my_count = 0;
    for (long long i = 0; i < my_niter; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if (x * x + y * y <= 1) my_count++;
    }

    if (rank != 0) {
        /*
         * BUFFERED SEND: MPI_Bsend copies the message into a buffer that WE provide,
         * then returns straight away, whether or not rank 0 has posted its receive.
         * The buffer must hold the packed message plus MPI_BSEND_OVERHEAD bytes.
         */
        int packed, bufsize;
        MPI_Pack_size(1, MPI_LONG_LONG, MPI_COMM_WORLD, &packed);
        bufsize = packed + MPI_BSEND_OVERHEAD;
        char *buffer = (char *)malloc(bufsize);
        MPI_Buffer_attach(buffer, bufsize);

        MPI_Bsend(&my_count, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);

        /* Detach waits until the buffered message has actually been sent, then frees it */
        MPI_Buffer_detach(&buffer, &bufsize);
        free(buffer);
    } else {
        long long total_count = my_count;
        int order[64], n = 0;
        for (int k = 1; k < size; k++) {
            long long part;
            MPI_Status status;
            MPI_Recv(&part, 1, MPI_LONG_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            total_count += part;
            if (n < 64) order[n++] = status.MPI_SOURCE;
        }
        double elapsed = MPI_Wtime() - start;

        double pi = (double)total_count / NITER * 4;
        printf("Processes        = %d\n", size);
        printf("# of trials      = %lld\n", NITER);
        printf("Points inside    = %lld\n", total_count);
        printf("Estimate of pi   = %.10f\n", pi);
        printf("Error vs M_PI    = %.10f\n", fabs(pi - M_PI));
        printf("Time             = %.6f sec\n", elapsed);
        printf("Receive order    =");
        for (int k = 0; k < n; k++) printf(" %d", order[k]);
        printf("%s\n", n ? "" : " (none, only one process)");
    }

    MPI_Finalize();
    return 0;
}
