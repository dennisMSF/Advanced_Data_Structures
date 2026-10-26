"""Vector search over the chunks table."""

import psycopg
from pgvector.psycopg import register_vector

from arxiv_rag.config import load_settings

settings = load_settings()

SEARCH_SQL = """
    SELECT paper_id, title, chunk_idx, text, 1 - (embedding <=> %(query)s) AS score
    FROM chunks
    ORDER BY embedding <=> %(query)s
    LIMIT %(k)s
"""


def search_chunks(query_vector, k: int) -> list[dict]:
    """Return the k chunks closest to the query by cosine similarity."""
    with psycopg.connect(settings.postgres_dsn) as conn:
        register_vector(conn)
        rows = conn.execute(SEARCH_SQL, {"query": query_vector, "k": k}).fetchall()
    return [
        {"paper_id": pid, "title": title, "chunk_idx": idx, "text": text, "score": float(score)}
        for pid, title, idx, text, score in rows
    ]
