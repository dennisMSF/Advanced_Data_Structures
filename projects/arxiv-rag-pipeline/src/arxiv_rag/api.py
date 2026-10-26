"""FastAPI service exposing semantic search over the arXiv chunk index."""

from contextlib import asynccontextmanager

from fastapi import FastAPI, Query
from sentence_transformers import SentenceTransformer

from arxiv_rag.config import load_settings
from arxiv_rag.search import search_chunks

settings = load_settings()


@asynccontextmanager
async def lifespan(app: FastAPI):
    # Load the model once at startup instead of on every request.
    app.state.model = SentenceTransformer(settings.embedding_model)
    yield


app = FastAPI(title="arXiv RAG retrieval", lifespan=lifespan)


@app.get("/search")
def search(
    q: str = Query(min_length=3, max_length=500),
    k: int = Query(default=5, ge=1, le=50),
):
    query_vector = app.state.model.encode(q, normalize_embeddings=True)
    return {"query": q, "results": search_chunks(query_vector, k)}


@app.get("/health")
def health():
    return {"status": "ok"}
