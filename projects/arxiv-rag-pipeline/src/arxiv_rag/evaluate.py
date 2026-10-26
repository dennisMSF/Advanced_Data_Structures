"""Measure retrieval quality: recall@k over a labeled question set."""

import json
import sys
from pathlib import Path

CUTOFFS = (1, 5, 10)
SEARCH_DEPTH = 30  # chunks retrieved per question, enough to yield 10 distinct papers


def recall_at_k(retrieved: dict[str, list[str]], relevant: dict[str, set[str]], k: int) -> float:
    """Average, over questions, of the share of relevant papers found in the top k results."""
    if not relevant:
        raise ValueError("no evaluation questions")
    scores = []
    for qid, gold in relevant.items():
        if not gold:
            raise ValueError(f"question {qid} has no relevant papers")
        top_k = set(retrieved.get(qid, [])[:k])
        scores.append(len(gold & top_k) / len(gold))
    return sum(scores) / len(scores)


def load_questions(path: Path) -> list[dict]:
    with path.open(encoding="utf-8") as f:
        return [json.loads(line) for line in f if line.strip()]


def main() -> None:
    # Imported here so the metric functions can be tested without the model or database stack.
    from sentence_transformers import SentenceTransformer

    from arxiv_rag.config import load_settings
    from arxiv_rag.search import search_chunks

    settings = load_settings()
    path = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("eval/questions.jsonl")
    questions = load_questions(path)
    model = SentenceTransformer(settings.embedding_model)

    relevant = {q["id"]: set(q["relevant_ids"]) for q in questions}
    retrieved: dict[str, list[str]] = {}
    for q in questions:
        vector = model.encode(q["question"], normalize_embeddings=True)
        chunks = search_chunks(vector, SEARCH_DEPTH)
        # Collapse chunk hits to distinct papers, keeping rank order.
        retrieved[q["id"]] = list(dict.fromkeys(c["paper_id"] for c in chunks))

    print(f"Questions: {len(questions)}")
    for k in CUTOFFS:
        print(f"recall@{k}: {recall_at_k(retrieved, relevant, k):.3f}")


if __name__ == "__main__":
    main()
