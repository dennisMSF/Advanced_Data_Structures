# arXiv RAG Pipeline

A local, zero-cost data pipeline that ingests arXiv paper abstracts, cleans them with PySpark, chunks and embeds them, loads them into PostgreSQL with pgvector, and serves semantic search through a FastAPI endpoint. Retrieval quality is measured against a labeled question set.

## Architecture

```
arXiv API
   │  ingest.py            (requests, rate-limited)
   ▼
data/raw/arxiv/ingest_date=YYYY-MM-DD/papers.jsonl     raw layer, append-only
   │  transform.py         (PySpark: trim, filter, dedupe)
   ▼
data/curated/arxiv_papers/*.parquet                    curated layer
   │  embed.py             (chunking + sentence-transformers)
   ▼
PostgreSQL + pgvector  (table `chunks`, HNSW index)    serving layer
   │  api.py  (GET /search)      evaluate.py  (recall@k)
   ▼
Clients / evaluation report
```

The raw and curated folders follow S3-style key partitioning (`ingest_date=...`), so the same layout can move to S3 and AWS Glue later with small changes.

## Stack

Python 3.11 · PySpark 3.5 · PostgreSQL 16 with pgvector · sentence-transformers (`all-MiniLM-L6-v2`) · FastAPI · pytest · GitHub Actions · Docker Compose

## Quick start

The full environment setup is in [docs/ENVIRONMENT.md](docs/ENVIRONMENT.md).

```bash
cp .env.example .env
make install     # creates .venv and installs pinned dependencies
make up          # starts PostgreSQL with pgvector
make ingest      # fetches paper metadata from arXiv into data/raw
make transform   # PySpark: raw -> curated Parquet
make embed       # chunks, embeds, and loads into pgvector
make serve       # API at http://127.0.0.1:8000/docs
make test        # unit tests (no Docker or Spark required)
make eval        # recall@k on eval/questions.jsonl
```

## Evaluation

`eval/questions.jsonl` holds one JSON object per line:

```json
{"id": "q001", "question": "...", "relevant_ids": ["2401.12345", "2305.67890"]}
```

`relevant_ids` are arXiv IDs from the indexed corpus. `make eval` reports recall@1, @5, and @10. For each question it takes the top 30 chunks, collapses them to distinct papers in rank order, and checks how many of the relevant papers appear in the top k. `eval/questions.example.jsonl` shows the format; replace its placeholder IDs with real ones before running the evaluation.

## Results

| Metric | Value | Notes |
|---|---|---|
| Papers ingested | TBD | `make ingest` output |
| Chunks indexed | TBD | `make embed` output |
| recall@5 | TBD | number of questions in the evaluation set |
| Query latency | TBD | p50 on local Apple M1, 8 GB |

Fill this table only with numbers produced by the commands above.

## Design decisions

- **Local-first, zero cost.** Everything runs in Docker and on the laptop. The only external calls are to the arXiv API and, once, to Hugging Face for the model download.
- **Cloud-ready layout.** Partitioned raw and curated folders and Parquet output map directly to S3, Glue, and Athena.
- **Retrieval before generation.** This version returns scored passages and measures retrieval quality. An answer-generation step can be added later without changing the index.
- **Full reload.** `embed.py` truncates and reloads the table. This keeps the first version simple and reproducible; incremental loads are a planned extension.
- **Title in the embedded text.** Each chunk is embedded with its paper title as context, which helps short queries find the right paper.

## Roadmap

- Airflow DAG to orchestrate ingest → transform → embed
- Great Expectations checks on the curated layer
- Terraform and an S3 + Glue deployment variant
- Answer generation with a local LLM (Ollama) or a hosted API
