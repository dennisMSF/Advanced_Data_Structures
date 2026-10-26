import pytest

from arxiv_rag.chunking import chunk_text


def test_windows_overlap_and_cover_all_words():
    text = " ".join(f"w{i}" for i in range(10))
    assert chunk_text(text, size=4, overlap=1) == [
        "w0 w1 w2 w3",
        "w3 w4 w5 w6",
        "w6 w7 w8 w9",
    ]


def test_empty_text_returns_no_chunks():
    assert chunk_text("   ") == []


def test_rejects_overlap_not_smaller_than_size():
    with pytest.raises(ValueError):
        chunk_text("a b c", size=2, overlap=2)
