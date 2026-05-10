from collections import deque
from dataclasses import dataclass


@dataclass
class Op:
    name: str
    inputs:  list[str]
    outputs: list[str]


def get_bidirectional_traverse(ops: list[Op]):
    produces  = {t: op.name for op in ops for t in op.outputs}
    out_edges = {op.name: [] for op in ops}
    in_edges = {op.name: []  for op in ops}

    for op in ops:
        for t in op.inputs:
            if t in produces:
                src = produces[t]
                out_edges[src].append(op.name)
                in_edges[op.name].append(src)

    return out_edges, in_edges


def topo_sort(ops: list[Op]) -> list[Op]:
    out_edges, in_edges = get_bidirectional_traverse(ops)
    by_name  = {op.name: op for op in ops}
    queue    = deque(op for op in ops if not in_edges[op.name])
    schedule = []

    while queue:
        op = queue.popleft()
        schedule.append(op)
        for dep in out_edges[op.name]:
            in_edges[dep].remove(op.name)
            if not in_edges[dep]: queue.append(by_name[dep])

    if len(schedule) != len(ops): raise ValueError("Cycle detected in graph")
    return schedule


def liveness(schedule: list[Op]) -> dict[str, tuple[int, int]]:
    birth = {t: i for i, op in enumerate(schedule) for t in op.outputs}
    death: dict[str, int] = {}

    for step, op in reversed(list(enumerate(schedule))):
        for t in op.inputs:
            if t in birth and t not in death:
                death[t] = step

    for t in birth:
        # if t not in death:
        #     death[t] = birth[t]
        death.setdefault(t, birth[t])

    return {t: (birth[t], death[t]) for t in birth}


def peak_memory(intervals, num_of_steps) -> list[int]:
    schedule_memory = [0] * num_of_steps
    for _, (birth, death) in intervals.items():
        for index in range(birth, death):
            schedule_memory[index] += 1
    return schedule_memory


if __name__ == "__main__":
    ops = [
        Op("norm",     inputs=["r"],      outputs=["out"]),
        Op("attn_out", inputs=["w","v"],  outputs=["a"]),
        Op("qkv",      inputs=["x"],      outputs=["q","k","v"]),
        Op("add",      inputs=["p","x"],  outputs=["r"]),
        Op("softmax",  inputs=["s"],      outputs=["w"]),
        Op("proj",     inputs=["a"],      outputs=["p"]),
        Op("scores",   inputs=["q","k"],  outputs=["s"]),
    ]

    schedule  = topo_sort(ops)
    print("Schedule:")
    for op in schedule: print(op)

    intervals = liveness(schedule)
    print("\nLiveness intervals:")
    for tensor, interval in intervals.items(): print(f"{tensor}: {interval}")

    schedule_memory = peak_memory(intervals, len(schedule))
    print("Peak memory usage:")
    print(max(schedule_memory))