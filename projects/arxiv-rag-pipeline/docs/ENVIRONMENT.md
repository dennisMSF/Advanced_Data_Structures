# Environment

This project runs entirely on a local machine. It needs no cloud account and costs nothing to operate.

## Where each part runs

| Component | Where | Why |
|---|---|---|
| Code, tests, Makefile | MacBook with Apple M1, 8 GB RAM, macOS 26 | Development machine |
| PostgreSQL with pgvector | Docker container on the Mac | Same engine a production RDS instance would use |
| PySpark | Local JVM in `local[*]` mode on the Mac | Enough for a corpus of thousands of abstracts |
| Embedding model | Downloaded once from Hugging Face, runs on CPU | Small model, about 90 MB |
| Unit tests | GitHub Actions (`ubuntu-latest`) | Pure Python, so no Docker or Spark is needed |
| Source control | GitHub | Portfolio and CI |

External network access is limited to the arXiv API (ingest), Hugging Face (first model download), and package indexes (PyPI and Docker Hub) during installation.

## Software

| Software | Version | Install |
|---|---|---|
| Homebrew | latest | https://brew.sh |
| Python | 3.11 | `brew install python@3.11` |
| Java (JDK) | 17 | `brew install openjdk@17` (PySpark 3.5 supports Java 8, 11, and 17) |
| Docker Desktop | latest, Apple silicon build | https://www.docker.com |
| Git and Make | any recent | included with Xcode Command Line Tools |

Java setup for PySpark. Add these lines to `~/.zshrc`, then open a new terminal:

```bash
export JAVA_HOME="$(brew --prefix openjdk@17)/libexec/openjdk.jdk/Contents/Home"
export PATH="$JAVA_HOME/bin:$PATH"
```

Check the versions:

```bash
python3.11 --version   # Python 3.11.x
java -version          # openjdk version "17.x"
docker --version
```

## Resource budget (8 GB RAM)

| Process | Approximate RAM | How to limit it |
|---|---|---|
| Docker Desktop VM (PostgreSQL) | 1 to 1.5 GB | Docker Desktop → Settings → Resources → Memory: 2 GB |
| PySpark driver | up to 2 GB | `spark.driver.memory` in `transform.py` |
| Python with sentence-transformers and torch | 1 to 2 GB | Lower `BATCH_SIZE` in `embed.py` if needed |
| macOS and open apps | 2 to 3 GB | Close browsers and other heavy apps during `make embed` |

Run one heavy step at a time. For the first run, keep `ARXIV_MAX_RESULTS` at 500.

## First-time setup

```bash
git clone <your-repo-url> arxiv-rag-pipeline
cd arxiv-rag-pipeline
cp .env.example .env
make install     # creates .venv and installs pinned dependencies
make up          # starts PostgreSQL with pgvector
```

Then run the pipeline in order:

```bash
make ingest
make transform
make embed
make serve
```

Confirm that the environment works:

```bash
docker compose ps                                        # postgres is running
.venv/bin/python -c "import pyspark, sentence_transformers"   # no output means success
make test                                                # all tests pass
```

## Configuration

Settings come from `.env` (see `.env.example`). Defaults are defined in `src/arxiv_rag/config.py`.

| Variable | Default | Purpose |
|---|---|---|
| `DATA_DIR` | `data` | Location of the raw and curated layers |
| `ARXIV_QUERY` | `cat:cs.IR` | arXiv search query |
| `ARXIV_MAX_RESULTS` | `500` | Papers fetched per ingest run |
| `EMBEDDING_MODEL` | `sentence-transformers/all-MiniLM-L6-v2` | Embedding model, 384 dimensions |
| `CHUNK_SIZE` | `200` | Words per chunk |
| `CHUNK_OVERLAP` | `40` | Words shared by consecutive chunks |
| `POSTGRES_DSN` | `postgresql://rag:rag@localhost:5432/rag` | Database connection string |

The `chunks.embedding` column is `VECTOR(384)`. If you change `EMBEDDING_MODEL` to a model with a different output size, update `db/init.sql` and reset the database as described below.

## Troubleshooting

- **PySpark fails with `JAVA_HOME is not set` or `Java gateway process exited`.** Java 17 is not on your PATH. Run `java -version` and check the `~/.zshrc` lines above.
- **`make ingest` fails with HTTP 429 or returns no papers.** The script waits 3 seconds between arXiv requests. Wait a few minutes and run it again.
- **Schema changes do not apply.** `db/init.sql` runs only when the PostgreSQL volume is created. To reset, run `docker compose down -v`, then `make up`, then `make embed`. This deletes the stored chunks.
- **`make embed` runs out of memory.** Close other apps, or lower `BATCH_SIZE` in `src/arxiv_rag/embed.py`.
- **The first `make embed` is slow.** The model downloads once (about 90 MB) and loads from the local cache after that.

## Cost and secrets

- No cloud accounts and no paid services.
- `.env`, `data/`, and `.venv/` are ignored by git. The defaults in `.env.example` are for local use only; do not reuse them anywhere else.
