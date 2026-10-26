"""Chunk curated papers, embed the chunks, and load them into pgvector."""

import psycopg
import pyarrow.dataset as ds
from pgvector.psycopg import register_vector
from sentence_transformers import SentenceTransformer

from arxiv_rag.chunking import chunk_text
from arxiv_rag.config import load_settings

BATCH_SIZE = 64


def load_papers(curated_dir: str) -> list[dict]:
    return ds.dataset(curated_dir, format="parquet").to_table().to_pylist()


def build_chunks(papers: list[dict], size: int, overlap: int) -> list[dict]:
    chunks = []
    for paper in papers:
        for idx, text in enumerate(chunk_text(paper["abstract"], size, overlap)):
            chunks.append(
                {"paper_id": paper["paper_id"], "title": paper["title"], "chunk_idx": idx, "text": text}
            )
    return chunks


def main() -> None:
    settings = load_settings()
    curated_dir = str((settings.data_dir / "curated" / "arxiv_papers").resolve())
    chunks = build_chunks(load_papers(curated_dir), settings.chunk_size, settings.chunk_overlap)
    print(f"Embedding {len(chunks)} chunks from {curated_dir}")

    model = SentenceTransformer(settings.embedding_model)
    with psycopg.connect(settings.postgres_dsn) as conn:
        register_vector(conn)
        conn.execute("TRUNCATE chunks")  # full reload keeps runs reproducible
        for start in range(0, len(chunks), BATCH_SIZE):
            batch = chunks[start : start + BATCH_SIZE]
            # The title goes into the embedded text so short queries can match the paper.
            vectors = model.encode(
                [f"{c['title']}. {c['text']}" for c in batch], normalize_embeddings=True
            )
            with conn.cursor() as cur:
                cur.executemany(
                    "INSERT INTO chunks (paper_id, title, chunk_idx, text, embedding) "
                    "VALUES (%s, %s, %s, %s, %s)",
                    [
                        (c["paper_id"], c["title"], c["chunk_idx"], c["text"], vec)
                        for c, vec in zip(batch, vectors)
                    ],
                )
        conn.commit()
    print(f"Loaded {len(chunks)} chunks into pgvector")


if __name__ == "__main__":
    main()
