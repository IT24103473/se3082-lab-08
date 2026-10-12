/* Monte Carlo estimate of Pi, parallelised with MPI point-to-point messages.
   Based on the serial monte_pi.c from the lab sheet link. */
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

    /* Share the NITER points between the processes; the first (NITER % size) ranks take one extra */
    long long my_niter = NITER / size + (rank < NITER % size ? 1 : 0);

    /*
     * Every process needs its OWN random sequence. If all ranks used srand(SEED)
     * they would all test the same points and the extra work would be useless.
     * With one process this is srand(SEED), the same as the serial program.
     */
    srand(SEED + rank);

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    long long my_count = 0;             /* points inside the quarter circle */
    for (long long i = 0; i < my_niter; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if (x * x + y * y <= 1) my_count++;
    }

    if (rank != 0) {
        /* Workers send their count to rank 0 with MPI_Send (tag 0) */
        MPI_Send(&my_count, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    } else {
        /* Rank 0 receives from each worker, naming the source explicitly: 1, 2, 3, ... */
        long long total_count = my_count;
        int order[64], n = 0;
        for (int src = 1; src < size; src++) {
            long long part;
            MPI_Status status;
            MPI_Recv(&part, 1, MPI_LONG_LONG, src, 0, MPI_COMM_WORLD, &status);
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
