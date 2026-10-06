"""Render PlantUML sources without deleting existing generated images."""

from __future__ import annotations

import argparse
import os
import subprocess
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--jar",
        type=Path,
        default=os.environ.get("PLANTUML_JAR"),
        help="path to plantuml.jar (or set PLANTUML_JAR)",
    )
    parser.add_argument(
        "--source-dir",
        type=Path,
        default=Path("paper/diagrams"),
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path("paper/figures/diagrams"),
    )
    args = parser.parse_args()

    if args.jar is None:
        parser.error("--jar or PLANTUML_JAR is required")

    jar = Path(args.jar)
    if not jar.is_file():
        parser.error(f"PlantUML jar not found: {jar}")

    sources = sorted(args.source_dir.glob("*.puml"))
    if not sources:
        parser.error(f"No PlantUML sources found in {args.source_dir}")

    args.output_dir.mkdir(parents=True, exist_ok=True)

    for source in sources:
        output = args.output_dir / f"{source.stem}.png"
        result = subprocess.run(
            ["java", "-jar", str(jar), "-pipe", "-tpng"],
            input=source.read_bytes(),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )
        if result.returncode != 0:
            raise RuntimeError(
                f"PlantUML failed for {source}:\n{result.stderr.decode(errors='replace')}"
            )
        output.write_bytes(result.stdout)
        print(f"Rendered {source} -> {output}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
