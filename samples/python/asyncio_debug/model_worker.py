import time
from multiprocessing import Event, Queue
from queue import Empty

from metrics import MetricsClient
from model import Model, generate_with_model


def worker_proc(
    model: Model,
    request_in_q: Queue,
    response_out_q: Queue,
    metrics_client: MetricsClient,
    cancel: Event,
):
    model.load_weights()

    while not cancel.is_set() or not request_in_q.empty():
        try:
            req = request_in_q.get_nowait()
            # print(f"worker_proc: request_in_q attempt req: {req}")
        except Empty:
            time.sleep(1e-6)
            continue

        start_time = time.time()
        response = generate_with_model(model, req)
        end_time = time.time()

        print(f"worker_proc: pre log_metrics")
        metrics_client.log_metrics(end_time - start_time, req.num_new_tokens)
        response_out_q.put(response)

    response_out_q.close()
    response_out_q.join_thread()
    metrics_client.close()
