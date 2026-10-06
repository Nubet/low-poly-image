"""Plot point sets using the same xorshift32 generator as the C core."""

from __future__ import annotations

import argparse
from pathlib import Path

import matplotlib.pyplot as plt


MASK32 = 0xFFFFFFFF


def random_points(width: int, height: int, requested_count: int, seed: int) -> list[tuple[int, int]]:
    state = 0x6D2B79F5 if seed == 0 else seed & MASK32

    def next_value() -> int:
        nonlocal state
        value = state
        value = (value ^ ((value << 13) & MASK32)) & MASK32
        value = (value ^ (value >> 17)) & MASK32
        value = (value ^ ((value << 5) & MASK32)) & MASK32
        state = value
        return value

    points = [(0, 0), (width - 1, 0), (width - 1, height - 1), (0, height - 1)]
    points.extend((next_value() % width, next_value() % height) for _ in range(requested_count - 4))
    return points


def draw_set(ax, points: list[tuple[int, int]], title: str, width: int, height: int) -> None:
    corners = points[:4]
    random = points[4:]
    ax.scatter([point[0] for point in random], [point[1] for point in random], s=4, alpha=0.45)
    ax.scatter(
        [point[0] for point in corners],
        [point[1] for point in corners],
        s=35,
        color="#dc2626",
        marker="s",
        label="image corners",
        zorder=3,
    )
    ax.set_title(title, fontsize=11, weight="bold")
    ax.set_xlim(-1, width)
    ax.set_ylim(height, -1)
    ax.set_aspect("equal")
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.grid(True, alpha=0.2)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, default=Path("paper/figures/plots"))
    parser.add_argument("--width", type=int, default=160)
    parser.add_argument("--height", type=int, default=120)
    parser.add_argument("--seed", type=int, default=123)
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)

    fig, axes = plt.subplots(1, 2, figsize=(9, 4.5), constrained_layout=True)
    draw_set(axes[0], random_points(args.width, args.height, 500, args.seed), "500 points", args.width, args.height)
    draw_set(axes[1], random_points(args.width, args.height, 2000, args.seed), "2000 points", args.width, args.height)
    axes[0].legend(loc="lower right", fontsize=8)
    fig.suptitle(f"Point generation with xorshift32 seed {args.seed}", weight="bold")
    fig.savefig(args.output_dir / "point_sets_seed_123.png", dpi=180, bbox_inches="tight")
    plt.close(fig)

    fig, axes = plt.subplots(1, 2, figsize=(9, 4.5), constrained_layout=True)
    draw_set(axes[0], random_points(args.width, args.height, 500, args.seed), f"seed {args.seed}", args.width, args.height)
    draw_set(axes[1], random_points(args.width, args.height, 500, args.seed + 1), f"seed {args.seed + 1}", args.width, args.height)
    fig.suptitle("Changing the seed changes the interior points", weight="bold")
    fig.savefig(args.output_dir / "point_sets_seed_comparison.png", dpi=180, bbox_inches="tight")
    plt.close(fig)
    print(f"Wrote point-set figures to {args.output_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
