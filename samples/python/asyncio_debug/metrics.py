import asyncio
from dataclasses import dataclass, field
from enum import IntEnum
from multiprocessing import Event
from queue import Empty, Queue
from uuid import UUID, uuid4


class MetricDetail(IntEnum):
    BASIC = 0
    DETAILED = 1


@dataclass
class Metric:
    name: str
    value: float
    detail: MetricDetail
    id: UUID = field(default_factory=uuid4)


async def metrics_processing_thread(
    metrics_q: Queue, results: dict[str, list[Metric]], cancel: Event
):
    print("metrics_processing_thread: Start")
    while not cancel.is_set() or not metrics_q.empty():
        try:
            item = await asyncio.to_thread(metrics_q.get, timeout=1)
            print("metrics_processing_thread: metrics_q.get() item attempted")
        except Empty:
            continue

        print(f"metrics_processing_thread: metrics_q.get() item got: {item}")
        for metric in item:
            if metric.name not in results:
                results[metric.name] = []
            results[metric.name].append(metric)


class MetricsClient:
    def __init__(
        self,
        out_queue: Queue,
        buffer_factor: int,
        metrics_level: MetricDetail = MetricDetail.BASIC,
    ):
        self.metrics_level = metrics_level
        self.out_queue = out_queue
        self.buffer = []
        self.buffer_factor = buffer_factor

    def send_metric(self, metric: Metric):
        # [KBK-TODO] This is bug causing keys to be missing from results.
        # if metric.detail > self.metrics_level:
        #     return

        print(f"send_metric: Executing detail level: {metric}")
        if metric.detail == MetricDetail.DETAILED:
            # this metric is detailed, buffer it
            self.buffer.append(metric)
            if len(self.buffer) < self.buffer_factor:
                return

        try:
            # send the current metric
            self.out_queue.put_nowait([metric])
            if self.buffer:
                # send the buffered metrics
                self.out_queue.put_nowait(self.buffer)
        except Full:
            # queue is full, drop the current metric and buffered metrics
            pass
        finally:
            # clear the buffer
            if len(self.buffer) > 0:
                self.buffer.clear()

    def log_metrics(self, duration: float, num_new_tokens: int):
        self.send_metric(
            Metric(
                id=uuid4(),
                name="num_requests",
                value=1,
                detail=MetricDetail.BASIC,
            )
        )
        self.send_metric(
            Metric(
                id=uuid4(),
                name="latency",
                value=duration,
                detail=MetricDetail.DETAILED,
            )
        )
        self.send_metric(
            Metric(
                id=uuid4(),
                name="num_new_tokens",
                value=num_new_tokens,
                detail=MetricDetail.DETAILED,
            )
        )

    def close(self):
        self.out_queue.close()
        self.out_queue.join_thread()
