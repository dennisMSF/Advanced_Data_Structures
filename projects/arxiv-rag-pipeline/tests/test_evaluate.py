import pytest

from arxiv_rag.evaluate import recall_at_k


def test_recall_is_averaged_over_questions():
    retrieved = {"q1": ["a", "b", "c"], "q2": ["x", "y"]}
    relevant = {"q1": {"a", "z"}, "q2": {"y"}}
    # q1 finds 1 of 2 relevant papers in the top 3 (0.5); q2 finds 1 of 1 (1.0).
    assert recall_at_k(retrieved, relevant, k=3) == pytest.approx(0.75)


def test_recall_respects_the_cutoff():
    retrieved = {"q1": ["a", "b", "c"]}
    relevant = {"q1": {"c"}}
    assert recall_at_k(retrieved, relevant, k=2) == 0.0
    assert recall_at_k(retrieved, relevant, k=3) == 1.0


def test_question_without_relevant_papers_is_an_error():
    with pytest.raises(ValueError):
        recall_at_k({"q1": ["a"]}, {"q1": set()}, k=1)
