#include <mpi.h>
#include <stdio.h>

#define N 10000000LL

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /*
     * Split the numbers 1..N into one block per process.
     * If N does not divide evenly, the first (N % size) ranks take one extra
     * number each, so every number is added exactly once for any process count.
     */
    long long base  = N / size;
    long long extra = N % size;
    long long count = base + (rank < extra ? 1 : 0);
    long long first = rank * base + (rank < extra ? rank : extra) + 1;
    long long last  = first + count - 1;

    MPI_Barrier(MPI_COMM_WORLD);        /* start everyone together */
    double start = MPI_Wtime();

    long long local_sum = 0;
    for (long long i = first; i <= last; i++)
        local_sum += i;

    if (rank != 0) {
        /* Workers send their partial sum to rank 0 (tag 0) */
        MPI_Send(&local_sum, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    } else {
        /* Rank 0 adds its own part, then receives one part from each worker in rank order */
        long long total_sum = local_sum;
        for (int src = 1; src < size; src++) {
            long long part;
            MPI_Recv(&part, 1, MPI_LONG_LONG, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_sum += part;
        }
        double elapsed = MPI_Wtime() - start;

        long long expected = N * (N + 1) / 2;
        printf("Processes  = %d\n", size);
        printf("Total sum  = %lld\n", total_sum);
        printf("Expected   = %lld\n", expected);
        printf("Correct?   = %s\n", total_sum == expected ? "YES" : "NO");
        printf("Time       = %.6f sec\n", elapsed);
    }

    /* Each rank reports the block it added (printed after the timing so it does not affect it) */
    printf("  Rank %d: added %lld .. %lld (%lld numbers) => local_sum = %lld\n",
           rank, first, last, count, local_sum);

    MPI_Finalize();
    return 0;
}
