/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 *
 * Name: Kshitij Khode
 * Andrew ID: kkhode
 *
 * Note: Optimization hints taken from dlib's transpose implementation.
 *
 */
#include <stdio.h>
#include "cachelab.h"
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    REQUIRES(M > 0);
    REQUIRES(N > 0);

    /* Essentially do blocking and adjusted segmented moves */
    int in, out, in_b, out_b, rep, rep_b, rep_c, rep_d, rep_e, rep_f, rep_g, rep_h;
    if (M == 32 && N == 32) {
        for (in = 0; in < N; in += 8) {
            for (out = 0; out < M; out += 8) {
                for (in_b = in; (in_b < in+8) && (in_b < N); in_b++) {
                    for (out_b = out; (out_b < out+8) && (out_b < M); out_b++) {
                        if (in_b != out_b) B[out_b][in_b] = A[in_b][out_b];
                    }
                    if (in == out) B[in_b][in_b] = A[in_b][in_b];
                }
            }
        }
    }

    /* Essentially do blocking and adjusted segmented moves */
    if (M == 64 && N == 64) {
        for (out = 0; out < N; out += 8) {
            for (in = 0; in < M; in += 8) {
                rep = A[in][out+4];
                rep_b = A[in][out+5];
                rep_c = A[in][out+6];
                rep_d = A[in][out+7];
                rep_e = A[in+1][out+4];
                rep_f = A[in+1][out+5];
                rep_g = A[in+1][out+6];
                rep_h = A[in+1][out+7];
                for (in_b = in; (in_b < in+8) && (in_b < M); in_b++) {
                    for (out_b = out; (out_b < out+4) && (out_b < N); out_b++) {
                            B[out_b][in_b] = A[in_b][out_b];
                    }
                }
                for (in_b = in+7; in_b > in+1; in_b--) {
                    for (out_b = out+4; out_b < out+8; out_b++) {
                            B[out_b][in_b] = A[in_b][out_b];
                    }
                }
                B[out+4][in] = rep;
                B[out+5][in] = rep_b;
                B[out+6][in] = rep_c;
                B[out+7][in] = rep_d;
                B[out+4][in+1] = rep_e;
                B[out+5][in+1] = rep_f;
                B[out+6][in+1] = rep_g;
                B[out+7][in+1] = rep_h;
            }
        }
    }

    ENSURES(is_transpose(M, N, A, B));
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

    ENSURES(is_transpose(M, N, A, B));
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

