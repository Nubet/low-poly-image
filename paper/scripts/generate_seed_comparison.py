"""Generate reproducible CLI outputs for a seed comparison."""

from __future__ import annotations

import argparse
import hashlib
import subprocess
from pathlib import Path


def run_output(executable: Path, input_path: Path, output_path: Path, points: int, seed: int) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
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
    subprocess.run(command, check=True)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    digest.update(path.read_bytes())
    return digest.hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable", type=Path, required=True)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, default=Path("paper/figures/results"))
    parser.add_argument("--points", type=int, default=500)
    parser.add_argument("--same-seed", type=int, default=123)
    parser.add_argument("--different-seed", type=int, default=124)
    args = parser.parse_args()

    if not args.executable.is_file():
        parser.error(f"CLI executable not found: {args.executable}")
    if not args.input.is_file():
        parser.error(f"Input image not found: {args.input}")

    args.output_dir.mkdir(parents=True, exist_ok=True)
    same_a = args.output_dir / f"seed_{args.same_seed}_a.png"
    same_b = args.output_dir / f"seed_{args.same_seed}_b.png"
    different = args.output_dir / f"seed_{args.different_seed}.png"

    run_output(args.executable, args.input, same_a, args.points, args.same_seed)
    run_output(args.executable, args.input, same_b, args.points, args.same_seed)
    run_output(args.executable, args.input, different, args.points, args.different_seed)

    manifest = args.output_dir / "seed_comparison.txt"
    manifest.write_text(
        "Seed comparison\n"
        f"input={args.input}\n"
        f"points={args.points}\n"
        f"seed_a={args.same_seed}\n"
        f"seed_b={args.same_seed}\n"
        f"seed_different={args.different_seed}\n"
        f"sha256_a={sha256(same_a)}\n"
        f"sha256_b={sha256(same_b)}\n"
        f"sha256_different={sha256(different)}\n",
        encoding="utf-8",
    )
    print(f"Same seed identical: {sha256(same_a) == sha256(same_b)}")
    print(f"Wrote seed comparison to {args.output_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
