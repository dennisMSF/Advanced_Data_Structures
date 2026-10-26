"""Split text into overlapping word windows for embedding."""


def chunk_text(text: str, size: int = 200, overlap: int = 40) -> list[str]:
    """Return windows of `size` words; consecutive windows share `overlap` words."""
    if size <= 0:
        raise ValueError("size must be positive")
    if not 0 <= overlap < size:
        raise ValueError("overlap must be >= 0 and smaller than size")

    words = text.split()
    step = size - overlap
    chunks = []
    for start in range(0, len(words), step):
        chunks.append(" ".join(words[start : start + size]))
        if start + size >= len(words):
            break
    return chunks
