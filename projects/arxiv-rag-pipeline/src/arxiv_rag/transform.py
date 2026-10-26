"""Clean the raw arXiv layer with PySpark and write curated Parquet."""

from pyspark.sql import DataFrame, SparkSession
from pyspark.sql import functions as F

from arxiv_rag.config import load_settings

MIN_ABSTRACT_CHARS = 50


def build_spark() -> SparkSession:
    return (
        SparkSession.builder.appName("arxiv-rag-transform")
        .master("local[*]")
        .config("spark.driver.memory", "2g")
        .getOrCreate()
    )


def clean(raw: DataFrame) -> DataFrame:
    """Trim text, drop empty or very short abstracts, and remove duplicate paper IDs."""
    return (
        raw.withColumn("title", F.trim("title"))
        .withColumn("abstract", F.trim("abstract"))
        .filter(F.length("abstract") >= MIN_ABSTRACT_CHARS)
        .dropDuplicates(["paper_id"])
        .select("paper_id", "title", "abstract", "published", "authors")
    )


def main() -> None:
    settings = load_settings()
    raw_glob = str((settings.data_dir / "raw" / "arxiv" / "*" / "papers.jsonl").resolve())
    out_dir = (settings.data_dir / "curated" / "arxiv_papers").resolve()

    spark = build_spark()
    try:
        curated = clean(spark.read.json(raw_glob))
        curated.write.mode("overwrite").parquet(str(out_dir))
        count = spark.read.parquet(str(out_dir)).count()
    finally:
        spark.stop()
    print(f"Curated {count} papers to {out_dir}")


if __name__ == "__main__":
    main()
