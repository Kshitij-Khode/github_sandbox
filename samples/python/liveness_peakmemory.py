"""
Schedule liveness analysis and peak memory estimation.

Liveness tracks when each tensor is first produced (birth) and last consumed
(death). Peak memory is the maximum number of tensors simultaneously live at
any step in the schedule.

Demo graph: single attention block
    x ──► qkv ──► scores ──► softmax ──► attn_out ──► proj ──► add ──► norm
                  (q,k)       (s)           (w,v)       (a)     (p,r)   (out)
"""

from nodes import Node, topo_sort


def liveness(schedule: list[Node]) -> dict[str, tuple[int, int]]:
    birth = {t: i for i, node in enumerate(schedule) for t in node.outputs}
    death: dict[str, int] = {}

    for step, node in reversed(list(enumerate(schedule))):
        for t in node.inputs:
            if t in birth and t not in death:
                death[t] = step

    for t in birth:
        death.setdefault(t, birth[t])

    return {t: (birth[t], death[t]) for t in birth}


def peak_memory(intervals: dict[str, tuple[int, int]], num_steps: int) -> list[int]:
    live_at = [0] * num_steps
    for _, (birth, death) in intervals.items():
        for i in range(birth, death):
            live_at[i] += 1
    return live_at


if __name__ == "__main__":
    nodes = [
        Node("norm",     op="layer_norm", inputs=["r"],     outputs=["out"]),
        Node("attn_out", op="matmul",     inputs=["w","v"], outputs=["a"]),
        Node("qkv",      op="linear",     inputs=["x"],     outputs=["q","k","v"]),
        Node("add",      op="add",        inputs=["p","x"], outputs=["r"]),
        Node("softmax",  op="softmax",    inputs=["s"],     outputs=["w"]),
        Node("proj",     op="linear",     inputs=["a"],     outputs=["p"]),
        Node("scores",   op="matmul",     inputs=["q","k"], outputs=["s"]),
    ]

    schedule = topo_sort(nodes)
    print("Schedule:")
    for node in schedule:
        print(f"  {node.name} ({node.op})")

    intervals = liveness(schedule)
    print("\nLiveness intervals (tensor: birth → death step):")
    for tensor, (b, d) in intervals.items():
        print(f"  {tensor}: {b} → {d}")

    mem = peak_memory(intervals, len(schedule))
    print(f"\nPeak memory: {max(mem)} tensors live simultaneously")
