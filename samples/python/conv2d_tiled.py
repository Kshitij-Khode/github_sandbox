"""
Naive vs tiled 2-D convolution with padding and stride.

Naive conv2d: for each output pixel (oi, oj) we slide the kernel over the
padded input. Adjacent output pixels reuse overlapping input rows, but
because we compute them one at a time those rows may be evicted between
outputs — wasted loads grow with kernel size and stride.

Tiled conv2d: process a T×T block of output pixels together. The input
patch they collectively read is (T-1)*stride+FH × (T-1)*stride+FW — for
small T and typical 3×3 kernels this fits in L1 and is shared across all
T² outputs in the tile, cutting redundant input loads by ~T².

Output size formula:
    OH = (H + 2*padding - FH) // stride + 1
    OW = (W + 2*padding - FW) // stride + 1
"""

from __future__ import annotations


def _pad(inp: list[list[float]], padding: int) -> list[list[float]]:
    H, W = len(inp), len(inp[0])
    PH, PW = H + 2 * padding, W + 2 * padding
    out = [[0.0] * PW for _ in range(PH)]
    for i in range(H):
        for j in range(W):
            out[i + padding][j + padding] = inp[i][j]
    return out


def conv2d_naive(
    inp: list[list[float]],
    kernel: list[list[float]],
    padding: int = 0,
    stride: int = 1,
) -> list[list[float]]:
    H, W = len(inp), len(inp[0])
    FH, FW = len(kernel), len(kernel[0])
    OH = (H + 2 * padding - FH) // stride + 1
    OW = (W + 2 * padding - FW) // stride + 1
    padded = _pad(inp, padding)
    out = [[0.0] * OW for _ in range(OH)]
    for oi in range(OH):
        for oj in range(OW):
            acc = 0.0
            for fi in range(FH):
                for fj in range(FW):
                    acc += padded[oi * stride + fi][oj * stride + fj] * kernel[fi][fj]
            out[oi][oj] = acc
    return out


def conv2d_tiled(
    inp: list[list[float]],
    kernel: list[list[float]],
    padding: int = 0,
    stride: int = 1,
    T: int = 8,
) -> list[list[float]]:
    H, W = len(inp), len(inp[0])
    FH, FW = len(kernel), len(kernel[0])
    OH = (H + 2 * padding - FH) // stride + 1
    OW = (W + 2 * padding - FW) // stride + 1
    padded = _pad(inp, padding)
    out = [[0.0] * OW for _ in range(OH)]
    for oi0 in range(0, OH, T):
        for oj0 in range(0, OW, T):
            # input patch for this output tile stays in L1 across all T² outputs
            for oi in range(oi0, min(oi0 + T, OH)):
                for oj in range(oj0, min(oj0 + T, OW)):
                    acc = 0.0
                    for fi in range(FH):
                        for fj in range(FW):
                            acc += padded[oi * stride + fi][oj * stride + fj] * kernel[fi][fj]
                    out[oi][oj] = acc
    return out


if __name__ == "__main__":
    import random
    random.seed(0)

    H, W = 32, 32
    inp    = [[random.random() for _ in range(W)] for _ in range(H)]
    kernel = [[random.random() for _ in range(3)] for _ in range(3)]

    for padding, stride in [(0, 1), (1, 1), (1, 2)]:
        naive = conv2d_naive(inp, kernel, padding=padding, stride=stride)
        tiled = conv2d_tiled(inp, kernel, padding=padding, stride=stride, T=8)
        OH, OW = len(naive), len(naive[0])
        max_err = max(
            abs(naive[i][j] - tiled[i][j])
            for i in range(OH) for j in range(OW)
        )
        print(f"padding={padding} stride={stride}  output={OH}×{OW}  max_err={max_err:.2e}")
        assert max_err < 1e-9, "results diverged"

    print("conv2d_tiled correct.")
