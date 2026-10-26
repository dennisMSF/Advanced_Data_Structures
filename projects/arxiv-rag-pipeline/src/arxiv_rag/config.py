"""Runtime settings, read from environment variables with local-development defaults."""

import os
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class Settings:
    data_dir: Path
    arxiv_query: str
    arxiv_max_results: int
    embedding_model: str
    chunk_size: int
    chunk_overlap: int
    postgres_dsn: str


def load_settings() -> Settings:
    return Settings(
        data_dir=Path(os.getenv("DATA_DIR", "data")),
        arxiv_query=os.getenv("ARXIV_QUERY", "cat:cs.IR"),
        arxiv_max_results=int(os.getenv("ARXIV_MAX_RESULTS", "500")),
        embedding_model=os.getenv("EMBEDDING_MODEL", "sentence-transformers/all-MiniLM-L6-v2"),
        chunk_size=int(os.getenv("CHUNK_SIZE", "200")),
        chunk_overlap=int(os.getenv("CHUNK_OVERLAP", "40")),
        postgres_dsn=os.getenv("POSTGRES_DSN", "postgresql://rag:rag@localhost:5432/rag"),
    )
