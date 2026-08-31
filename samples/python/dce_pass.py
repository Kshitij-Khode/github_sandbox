"""
Dead Code Elimination (DCE) pass — remove nodes not reachable from outputs.

Any node whose outputs are never consumed by a live node is dead and can be
dropped. Walk backward from the designated output tensors; anything not
visited is unreachable and removed from the graph in-place.

Demo graph — two independent chains sharing input 'x', only one is live:

    x ──► relu ──► gelu ──► out      (live — reachable from output)
    x ──► log  ──► exp               (dead — nothing consumes 'exp')
"""

from __future__ import annotations
from nodes import Node


def dce(nodes: list[Node], output_tensors: list[str]) -> None:
    """
    Remove dead nodes from `nodes` in-place.

    Walks backward from `output_tensors` via a BFS over the producer map;
    any node not reached is dead and is removed.

    Time:  O(V + E)
    Space: O(V)
    """
    # tensor_name → node that produces it
    producer: dict[str, Node] = {t: n for n in nodes for t in n.outputs}

    visited: set[str] = set()   # node names already enqueued
    queue: list[Node] = []

    for t in output_tensors:
        if t in producer and producer[t].name not in visited:
            visited.add(producer[t].name)
            queue.append(producer[t])

    while queue:
        node = queue.pop()
        for t in node.inputs:
            if t in producer and producer[t].name not in visited:
                visited.add(producer[t].name)
                queue.append(producer[t])

    nodes[:] = [n for n in nodes if n.name in visited]


if __name__ == "__main__":
    nodes = [
        Node("x",    op="input",  inputs=[],       outputs=["x"]),
        Node("relu", op="relu",   inputs=["x"],     outputs=["a"]),
        Node("gelu", op="gelu",   inputs=["a"],     outputs=["out"]),
        Node("log",  op="log",    inputs=["x"],     outputs=["b"]),   # dead
        Node("exp",  op="exp",    inputs=["b"],     outputs=["c"]),   # dead
    ]

    print(f"Nodes before DCE ({len(nodes)}): {[n.name for n in nodes]}")
    dce(nodes, output_tensors=["out"])
    print(f"Nodes after  DCE ({len(nodes)}): {[n.name for n in nodes]}")

    assert all(n.name in {"x", "relu", "gelu"} for n in nodes), "unexpected survivors"
    print("DCE pass correct.")
