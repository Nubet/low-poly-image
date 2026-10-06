"""Summarize runtime benchmark CSV data and generate a plot."""

import argparse
import csv
import statistics
from collections import defaultdict
from pathlib import Path

import matplotlib.pyplot as plt


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--summary", type=Path, required=True)
    parser.add_argument("--plot", type=Path, required=True)
    return parser.parse_args()


def load_measurements(path: Path) -> dict[int, list[float]]:
    measurements: dict[int, list[float]] = defaultdict(list)
    with path.open(newline="", encoding="utf-8") as csv_file:
        for row in csv.DictReader(csv_file):
            measurements[int(row["points"])].append(float(row["elapsed_ms"]))
    return dict(measurements)


def main() -> None:
    args = parse_args()
    measurements = load_measurements(args.input)
    if not measurements:
        raise SystemExit("benchmark CSV contains no measurements")

    args.summary.parent.mkdir(parents=True, exist_ok=True)
    with args.summary.open("w", newline="", encoding="utf-8") as csv_file:
        fieldnames = ("points", "runs", "median_ms", "mean_ms", "stddev_ms")
        writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
        writer.writeheader()
        for points in sorted(measurements):
            values = measurements[points]
            writer.writerow(
                {
                    "points": points,
                    "runs": len(values),
                    "median_ms": f"{statistics.median(values):.3f}",
                    "mean_ms": f"{statistics.mean(values):.3f}",
                    "stddev_ms": f"{statistics.stdev(values) if len(values) > 1 else 0.0:.3f}",
                }
            )

    points = sorted(measurements)
    medians = [statistics.median(measurements[value]) for value in points]
    deviations = [statistics.stdev(measurements[value]) for value in points]

    figure, axis = plt.subplots(figsize=(7.2, 4.4))
    axis.errorbar(
        points,
        medians,
        yerr=deviations,
        color="#2563eb",
        marker="o",
        capsize=4,
        linewidth=2,
        label="Measured median",
    )
    axis.set_xscale("log", base=2)
    axis.set_xticks(points)
    axis.set_xticklabels([str(value) for value in points])
    axis.set_xlabel("Requested points")
    axis.set_ylabel("End-to-end runtime (ms)")
    axis.set_title("Lowpoly CLI runtime by point count")
    axis.grid(True, alpha=0.25)
    axis.legend()
    figure.tight_layout()
    args.plot.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(args.plot, dpi=180)
    print(f"wrote {args.summary}")
    print(f"wrote {args.plot}")


if __name__ == "__main__":
    main()
