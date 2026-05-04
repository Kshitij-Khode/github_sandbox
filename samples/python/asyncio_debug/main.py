import asyncio
from multiprocessing import Event, Process, Queue
from uuid import uuid4

from interfaces import Request, Response
from metrics import metrics_processing_thread
from model import Model
from model_worker import MetricsClient, worker_proc


async def send_requests(
    request_q: Queue, response_q: Queue, num_requests: int, num_new_tokens: int
) -> list[Response]:
    for i in range(num_requests):
        request_q.put_nowait(
            Request(id=uuid4(), prompt=[1, 2, 3], num_new_tokens=num_new_tokens)
        )

    print("send_requests: mid")

    responses = []
    for i in range(num_requests):
        print("send_requests: num_requests loop")
        response = await asyncio.to_thread(response_q.get, timeout=5)
        responses.append(response)
    return responses


async def main(
    num_requests: int,
    num_new_tokens: int,
    num_layers: int,
    vocab_size: int,
    hidden_size: int,
):
    """
    This is a simulation of a worker process that sends metrics to a main
    process. The worker will either send the metric directly if it is labelled
    "basic", or it will buffer and batch multiple metrics if they're labelled "detailed".

    It's expected that the main process will receive all of the metrics sent by
    the worker process exactly once, however they may be received in a different order.

    If you try running this script, you'll find that the output is incorrect.
    Your job is to identify the bug and fix it.
    """

    # start metrics processing thread
    metrics_q = Queue()
    metrics_results = {}
    cancel = Event()
    # metrics_results <- metrics_q <- metrics_client: MetricsClient() <- request_q or response_q of proc <- send_requests
    metrics_task = asyncio.create_task(
        metrics_processing_thread(metrics_q, metrics_results, cancel)
    )

    print("Stage: 0")
    # start model worker process
    request_q = Queue()
    response_q = Queue()
    metrics_client = MetricsClient(metrics_q, buffer_factor=20)
    model = Model(num_layers, vocab_size, hidden_size)
    proc = Process(
        target=worker_proc,
        args=(model, request_q, response_q, metrics_client, cancel),
    )
    proc.start()

    print("Stage: 1")
    responses = await send_requests(
        request_q, response_q, num_requests, num_new_tokens
    )

    # cancel metrics processing thread and join model worker process
    print("Stage: 2")
    cancel.set()
    proc.join()
    print("Stage: 3")
    await metrics_task

    expected_keys = {"latency", "num_new_tokens", "num_requests"}
    actual_keys = set(metrics_results.keys())
    if expected_keys != actual_keys:
        # [KBK-TODO] Search for "KBK-TODO" to find the source of this bug.
        print(f"❌ FAILED: Expected metrics with keys  {expected_keys}, got {actual_keys}")
        return

    for key in expected_keys:
        if len(metrics_results[key]) != num_requests:
            print(
                f"❌ FAILED: Expected {num_requests} metrics for {key}, got {len(metrics_results[key])}"
            )
            return

    print("✅ SUCCESS!!! Nice work")

if __name__ == "__main__":
    asyncio.run(
        main(
            num_requests=20,
            num_new_tokens=10,
            num_layers=2,
            vocab_size=100,
            hidden_size=10,
        )
    )
