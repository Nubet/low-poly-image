"""Measure the real lowpoly CLI runtime for increasing point counts."""

import argparse
import csv
import subprocess
import tempfile
import time
from pathlib import Path


DEFAULT_POINTS = (250, 500, 1000, 2000, 4000, 8000)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable", type=Path, required=True)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--seed", type=int, default=123)
    parser.add_argument("--repetitions", type=int, default=10)
    parser.add_argument("--warmups", type=int, default=2)
    parser.add_argument("--points", type=int, nargs="+", default=DEFAULT_POINTS)
    return parser.parse_args()


def run_once(executable: Path, input_path: Path, output_path: Path, points: int, seed: int) -> float:
    command = [
        str(executable),
        str(input_path),
        "--output",
        str(output_path),
        "--points",
        str(points),
        "--seed",
        str(seed),
    ]
    start = time.perf_counter()
    subprocess.run(command, check=True, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    return (time.perf_counter() - start) * 1000.0


def main() -> None:
    args = parse_args()
    if args.repetitions < 1 or args.warmups < 0:
        raise SystemExit("repetitions must be positive and warmups cannot be negative")
    if not args.executable.is_file():
        raise SystemExit(f"executable not found: {args.executable}")
    if not args.input.is_file():
        raise SystemExit(f"input image not found: {args.input}")

    args.output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="lowpoly-benchmark-") as temporary_directory:
        output_path = Path(temporary_directory) / "result.png"
        for points in args.points:
            for _ in range(args.warmups):
                run_once(args.executable, args.input, output_path, points, args.seed)

        with args.output.open("w", newline="", encoding="utf-8") as csv_file:
            writer = csv.DictWriter(
                csv_file,
                fieldnames=("points", "repetition", "seed", "elapsed_ms"),
            )
            writer.writeheader()
            for points in args.points:
                for repetition in range(1, args.repetitions + 1):
                    elapsed_ms = run_once(args.executable, args.input, output_path, points, args.seed)
                    writer.writerow(
                        {
                            "points": points,
                            "repetition": repetition,
                            "seed": args.seed,
                            "elapsed_ms": f"{elapsed_ms:.3f}",
                        }
                    )
                    csv_file.flush()
                    print(f"points={points} repetition={repetition} elapsed_ms={elapsed_ms:.3f}")


if __name__ == "__main__":
    main()
