-include .env
export

PY := .venv/bin/python
RUN := PYTHONPATH=src $(PY) -m

.PHONY: install up down ingest transform embed serve eval test

install:
	python3.11 -m venv .venv
	$(PY) -m pip install --upgrade pip
	$(PY) -m pip install -r requirements.txt -r requirements-dev.txt

up:
	docker compose up -d

down:
	docker compose down

ingest:
	$(RUN) arxiv_rag.ingest

transform:
	$(RUN) arxiv_rag.transform

embed:
	$(RUN) arxiv_rag.embed

serve:
	PYTHONPATH=src $(PY) -m uvicorn arxiv_rag.api:app --host 127.0.0.1 --port 8000 --reload

eval:
	$(RUN) arxiv_rag.evaluate eval/questions.jsonl

test:
	$(PY) -m pytest
