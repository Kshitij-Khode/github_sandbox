from dataclasses import dataclass, field
from uuid import UUID, uuid4


@dataclass
class Request:
    prompt: list[int]
    num_new_tokens: int
    id: UUID = field(default_factory=uuid4)


@dataclass
class Response:
    id: UUID
    response: list[int]
