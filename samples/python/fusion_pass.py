"""
Operator Fusion Pass — merge maximal runs of elementwise ops into single fused nodes.

Rule: stop fusing across a node whose output tensor has more than one consumer,
because fusing there would require replaying the fused kernel on each branch,
duplicating work instead of saving it.

Demo graph (two branches split at 'scale', which has 2 consumers):

    x → relu → gelu → scale ─┬─► bias_add → sigmoid → out1
                              └─► mul      → tanh    → out2

Expected fusion groups:
  Fused([relu, gelu, scale])   — scale ends the group (2 consumers downstream)
  Fused([bias_add, sigmoid])   — fresh group after the split
  Fused([mul, tanh])           — fresh group on the other branch
"""

from nodes import Node, topo_sort


ELEMENTWISE_OPS = {"relu", "gelu", "scale", "bias_add", "sigmoid", "mul", "tanh", "add"}


def fusion_pass(nodes: list[Node], elementwise_ops: set[str] = ELEMENTWISE_OPS) -> list[Node]:
    """
    Walk the schedule and greedily merge contiguous elementwise nodes into fused groups.

    A group closes when the current node's output has >1 consumer — continuing
    past it would duplicate the fused kernel on each branch.

    Returns a new node list where multi-node groups are replaced by a single
    FusedNode whose inputs/outputs span the group boundary.

    Time:  O(V + E)
    Space: O(V)
    """
    schedule = topo_sort(nodes)

    # count how many ops consume each tensor
    consumer_count: dict[str, int] = {}
    for node in nodes:
        for t in node.inputs:
            consumer_count[t] = consumer_count.get(t, 0) + 1

    groups: list[list[Node]] = []
    current: list[Node] = []

    for node in schedule:
        if node.op not in elementwise_ops:
            if current:
                groups.append(current)
                current = []
            groups.append([node])
            continue

        # If the previous group's tail produces a multi-consumer tensor, close it.
        if current and any(consumer_count.get(t, 0) > 1 for t in current[-1].outputs):
            groups.append(current)
            current = []

        current.append(node)

    if current:
        groups.append(current)

    result: list[Node] = []
    for group in groups:
        if len(group) == 1:
            result.append(group[0])
        else:
            result.append(Node(
                name="fused(" + ", ".join(n.name for n in group) + ")",
                op="fused[" + ", ".join(n.op for n in group) + "]",
                inputs=group[0].inputs,
                outputs=group[-1].outputs,
            ))
    return result


if __name__ == "__main__":
    nodes = [
        Node("relu",     op="relu",     inputs=["x"],   outputs=["a"]),
        Node("gelu",     op="gelu",     inputs=["a"],   outputs=["b"]),
        Node("scale",    op="scale",    inputs=["b"],   outputs=["c"]),   # 2 consumers
        Node("bias_add", op="bias_add", inputs=["c"],   outputs=["d"]),
        Node("sigmoid",  op="sigmoid",  inputs=["d"],   outputs=["out1"]),
        Node("mul",      op="mul",      inputs=["c"],   outputs=["f"]),
        Node("tanh",     op="tanh",     inputs=["f"],   outputs=["out2"]),
    ]

    print("Before fusion:")
    for n in topo_sort(nodes):
        print(f"  {n.name} ({n.op})")

    fused = fusion_pass(nodes)
    print("\nAfter fusion:")
    for n in fused:
        print(f"  {n.name}")
        print(f"    inputs:  {n.inputs}")
        print(f"    outputs: {n.outputs}")
