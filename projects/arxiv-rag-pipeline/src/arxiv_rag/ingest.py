"""Fetch paper metadata and abstracts from the arXiv API into the raw layer."""

import json
import time
import xml.etree.ElementTree as ET
from datetime import date
from pathlib import Path

import requests

from arxiv_rag.config import load_settings

ARXIV_API = "https://export.arxiv.org/api/query"
ATOM = "{http://www.w3.org/2005/Atom}"
PAGE_SIZE = 100
DELAY_SECONDS = 3  # arXiv asks API clients to wait between requests


def parse_feed(xml_text: str) -> list[dict]:
    """Extract paper records from an arXiv Atom feed."""
    root = ET.fromstring(xml_text)
    papers = []
    for entry in root.findall(f"{ATOM}entry"):
        papers.append(
            {
                # IDs look like http://arxiv.org/abs/2401.12345v1 or http://arxiv.org/abs/cs/0101001v1
                "paper_id": entry.findtext(f"{ATOM}id", "").split("/abs/", 1)[-1],
                "title": " ".join(entry.findtext(f"{ATOM}title", "").split()),
                "abstract": " ".join(entry.findtext(f"{ATOM}summary", "").split()),
                "published": entry.findtext(f"{ATOM}published", ""),
                "authors": [a.findtext(f"{ATOM}name", "") for a in entry.findall(f"{ATOM}author")],
            }
        )
    return papers


def fetch_all(query: str, max_results: int) -> list[dict]:
    """Page through the arXiv API, waiting between requests."""
    papers: list[dict] = []
    for start in range(0, max_results, PAGE_SIZE):
        if start:
            time.sleep(DELAY_SECONDS)
        response = requests.get(
            ARXIV_API,
            params={
                "search_query": query,
                "start": start,
                "max_results": min(PAGE_SIZE, max_results - start),
                "sortBy": "submittedDate",
                "sortOrder": "descending",
            },
            timeout=30,
        )
        response.raise_for_status()
        page = parse_feed(response.text)
        if not page:
            break
        papers.extend(page)
    return papers


def write_raw(papers: list[dict], data_dir: Path) -> Path:
    """Write one JSON Lines file per run, partitioned by ingest date."""
    partition = data_dir / "raw" / "arxiv" / f"ingest_date={date.today().isoformat()}"
    partition.mkdir(parents=True, exist_ok=True)
    out = partition / "papers.jsonl"
    with out.open("w", encoding="utf-8") as f:
        for paper in papers:
            f.write(json.dumps(paper, ensure_ascii=False) + "\n")
    return out


def main() -> None:
    settings = load_settings()
    papers = fetch_all(settings.arxiv_query, settings.arxiv_max_results)
    out = write_raw(papers, settings.data_dir)
    print(f"Wrote {len(papers)} papers to {out}")


if __name__ == "__main__":
    main()
