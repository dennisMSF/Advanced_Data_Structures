CREATE EXTENSION IF NOT EXISTS vector;

CREATE TABLE IF NOT EXISTS chunks (
    id         BIGSERIAL PRIMARY KEY,
    paper_id   TEXT NOT NULL,
    title      TEXT NOT NULL,
    chunk_idx  INTEGER NOT NULL,
    text       TEXT NOT NULL,
    embedding  VECTOR(384) NOT NULL
);

CREATE INDEX IF NOT EXISTS chunks_embedding_idx
    ON chunks USING hnsw (embedding vector_cosine_ops);
