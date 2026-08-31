"""
Naive vs tiled matrix multiplication.

Naive matmul (i,j,k order): the inner loop strides through B column-wise —
B[k][j] steps by a full row in memory on each k increment. Once N exceeds
~L1_size/4 bytes, every B access is a cache miss: O(N^3) misses total.

Tiled matmul: divide A, B, C into T×T blocks. The inner k-tile keeps a T×T
slice of B resident in L1; it is reused T times before eviction. Cache misses
drop from O(N^3) to O(N^3/T). Rule of thumb: pick T so three tiles
(A-tile, B-tile, C-tile) fit in L1 — e.g. T=32 → 3×32×32×4 B = 12 KB for
a 32 KB L1. Larger T captures more reuse but risks L1 spill.
"""

from __future__ import annotations


def matmul_naive(A: list[list[float]], B: list[list[float]]) -> list[list[float]]:
    N, M, K = len(A), len(A[0]), len(B[0])
    C = [[0.0] * K for _ in range(N)]
    for i in range(N):
        for j in range(K):
            for k in range(M):
                C[i][j] += A[i][k] * B[k][j]   # B[k][j]: column-wise, cache-unfriendly
    return C


def matmul_tiled(A: list[list[float]], B: list[list[float]], T: int = 32) -> list[list[float]]:
    N, M, K = len(A), len(A[0]), len(B[0])
    C = [[0.0] * K for _ in range(N)]
    for i0 in range(0, N, T):
        for j0 in range(0, K, T):
            for k0 in range(0, M, T):
                for i in range(i0, min(i0 + T, N)):
                    for k in range(k0, min(k0 + T, M)):
                        a_ik = A[i][k]              # hoist: reused across entire j-tile
                        for j in range(j0, min(j0 + T, K)):
                            C[i][j] += a_ik * B[k][j]  # B[k][j0..j0+T]: contiguous row slice
    return C


if __name__ == "__main__":
    import random
    random.seed(0)
    N = 64
    A = [[random.random() for _ in range(N)] for _ in range(N)]
    B = [[random.random() for _ in range(N)] for _ in range(N)]

    C_naive = matmul_naive(A, B)
    C_tiled = matmul_tiled(A, B, T=16)

    max_err = max(
        abs(C_naive[i][j] - C_tiled[i][j])
        for i in range(N) for j in range(N)
    )
    print(f"Max element-wise error naive vs tiled: {max_err:.2e}")
    assert max_err < 1e-9, "results diverged"
    print("matmul_tiled correct.")
