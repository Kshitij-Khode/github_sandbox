from __future__ import annotations
from collections import deque
from dataclasses import dataclass, field


@dataclass
class Node:
    name: str
    op: str
    inputs:  list[str] = field(default_factory=list)  # tensor names consumed
    outputs: list[str] = field(default_factory=list)  # tensor names produced


def topo_sort(nodes: list[Node]) -> list[Node]:
    produces  = {t: node.name for node in nodes for t in node.outputs}
    out_edges = {node.name: [] for node in nodes}
    in_edges  = {node.name: [] for node in nodes}

    for node in nodes:
        for t in node.inputs:
            if t in produces:
                src = produces[t]
                out_edges[src].append(node.name)
                in_edges[node.name].append(src)

    by_name = {node.name: node for node in nodes}
    queue   = deque(node for node in nodes if not in_edges[node.name])
    order: list[Node] = []

    while queue:
        node = queue.popleft()
        order.append(node)
        for dep in out_edges[node.name]:
            in_edges[dep].remove(node.name)
            if not in_edges[dep]:
                queue.append(by_name[dep])

    if len(order) != len(nodes):
        raise ValueError("Cycle detected in graph")
    return order
