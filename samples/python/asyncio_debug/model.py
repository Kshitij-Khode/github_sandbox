import time
import torch
from interfaces import Request, Response


class Model(torch.nn.Module):
    def __init__(self, num_layers: int, vocab_size: int, hidden_size: int):
        super().__init__()
        self.num_layers = num_layers
        self.vocab_size = vocab_size
        self.hidden_size = hidden_size
        self.linears = torch.nn.ModuleList(
            [
                torch.nn.Linear(hidden_size, hidden_size)
                for _ in range(num_layers)
            ]
        )
        self.embeddings = torch.nn.Embedding(vocab_size, hidden_size)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        x = self.embeddings(x)
        for linear in self.linears:
            x = linear(x)
        return x

    def load_weights(self) -> None:
        time.sleep(2) # simulate model loading weights


def generate_with_model(model: Model, request: Request) -> Response:
    prompt_tensor = torch.tensor([request.prompt])
    for _ in range(request.num_new_tokens):
        logits = model(prompt_tensor)  # (batch_size, vocab_size)
        next_token = torch.argmax(logits[:, [-1], :], dim=-1)  # (batch_size)
        prompt_tensor = torch.cat(
            [prompt_tensor, next_token], dim=-1
        )  # (batch_size, prompt_length + 1)
    return Response(id=request.id, response=prompt_tensor.tolist()[0])
