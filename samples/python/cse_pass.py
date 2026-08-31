"""
Common Subexpression Elimination (CSE) pass on a tensor-name DAG.

Motivating example: Transformer QKV projection where the framework traces
layer_norm(x) three times — once per projection — as separate nodes.

    x ──► LN_1 ──► Linear_Q ──► q
    x ──► LN_2 ──► Linear_K ──► k
    x ──► LN_3 ──► Linear_V ──► v

CSE detects LN_1/LN_2/LN_3 are structurally identical (same op, same inputs)
and merges them into one node, redirecting all consumers.

    x ──► LN ──┬──► Linear_Q ──► q
               ├──► Linear_K ──► k
               └──► Linear_V ──► v

Downside: the merged node now has 3 consumers, so a downstream fusion pass
won't fuse LN→Linear (fusing would duplicate work). CSE trades compute
savings for potential fusion opportunities and higher peak memory (merged
output must stay live until the last consumer).
"""

from __future__ import annotations
from nodes import Node, topo_sort


def cse(nodes: list[Node]) -> None:
    """
    Run CSE over the graph in-place, removing duplicate nodes from `nodes`.

    Algorithm:
      - Topological order traversal.
      - For each node resolve its inputs through a canonical tensor-name map,
        then compute a structural key: (op, tuple-of-resolved-input-names).
      - If the key was already seen, map this node's outputs to the survivor's
        outputs and remove the node from `nodes`.
      - Surviving nodes have their inputs rewritten to resolved names.

    Time:  O(V + E)
    Space: O(V)
    """
    schedule = topo_sort(nodes)

    # tensor_name → canonical tensor_name (survivor's output)
    origtensor_to_survtensor: dict[str, str] = {}
    survivors: dict[tuple, Node] = {}

    for node in schedule:
        resolved = tuple(origtensor_to_survtensor.get(t, t) for t in node.inputs)
        key = (node.op, resolved)

        if key in survivors:
            nodes.remove(node)
            for dup_out, surv_out in zip(node.outputs, survivors[key].outputs):
                origtensor_to_survtensor[dup_out] = surv_out
        else:
            node.inputs = list(resolved)
            survivors[key] = node


# ---------------------------------------------------------------------------
# Demo: QKV projection (3 redundant layer-norm nodes → 1 after CSE)
# ---------------------------------------------------------------------------

if __name__ == "__main__":
    nodes = [
        Node("x",    op="input",      inputs=[],            outputs=["x"]),
        Node("LN_1", op="layer_norm", inputs=["x"],         outputs=["ln1"]),
        Node("LN_2", op="layer_norm", inputs=["x"],         outputs=["ln2"]),
        Node("LN_3", op="layer_norm", inputs=["x"],         outputs=["ln3"]),
        Node("W_q",  op="weight_q",   inputs=[],            outputs=["wq"]),
        Node("W_k",  op="weight_k",   inputs=[],            outputs=["wk"]),
        Node("W_v",  op="weight_v",   inputs=[],            outputs=["wv"]),
        Node("q",    op="linear",     inputs=["ln1","wq"],  outputs=["q"]),
        Node("k",    op="linear",     inputs=["ln2","wk"],  outputs=["k"]),
        Node("v",    op="linear",     inputs=["ln3","wv"],  outputs=["v"]),
    ]

    print(f"Nodes before CSE: {len(nodes)}")
    cse(nodes)
    print(f"Nodes after  CSE: {len(nodes)}")

    q_node = next(n for n in nodes if n.name == "q")
    k_node = next(n for n in nodes if n.name == "k")
    v_node = next(n for n in nodes if n.name == "v")
    assert q_node.inputs[0] == k_node.inputs[0] == v_node.inputs[0], "LN not merged"
    print(f"All projections consume tensor '{q_node.inputs[0]}' — CSE pass correct.")
