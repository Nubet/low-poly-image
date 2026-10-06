"""Generate geometry figures used to explain the rendering algorithm."""

from __future__ import annotations

import argparse
from pathlib import Path

import matplotlib.pyplot as plt
from matplotlib.patches import Circle, Polygon, Rectangle


def style_axes(ax, title: str, xlim: tuple[float, float], ylim: tuple[float, float]) -> None:
    ax.set_title(title, fontsize=11, weight="bold")
    ax.set_aspect("equal")
    ax.set_xlim(*xlim)
    ax.set_ylim(*ylim)
    ax.grid(True, alpha=0.2)
    ax.set_xlabel("x")
    ax.set_ylabel("y")


def save_orientation(output: Path) -> None:
    fig, axes = plt.subplots(1, 2, figsize=(8, 3.8), constrained_layout=True)
    cases = [
        ((0, 0), (4, 0), (1.5, 3), "positive orientation"),
        ((0, 0), (1.5, 3), (4, 0), "negative orientation"),
    ]

    for ax, (a, b, c, label) in zip(axes, cases):
        ax.add_patch(Polygon([a, b, c], closed=True, facecolor="#bae6fd", edgecolor="#0369a1"))
        ax.annotate("", xy=b, xytext=a, arrowprops={"arrowstyle": "->", "color": "#0f172a"})
        ax.annotate("", xy=c, xytext=b, arrowprops={"arrowstyle": "->", "color": "#0f172a"})
        for point, name in ((a, "a"), (b, "b"), (c, "c")):
            ax.scatter(*point, color="#0f172a", zorder=3)
            ax.text(point[0] + 0.12, point[1] + 0.12, name)
        style_axes(ax, label, (-1, 5), (-1, 4))

    fig.savefig(output, dpi=180, bbox_inches="tight")
    plt.close(fig)


def save_centroid(output: Path) -> None:
    fig, ax = plt.subplots(figsize=(6, 4.5), constrained_layout=True)
    ax.set_facecolor("#f8fafc")
    ax.add_patch(Rectangle((0, 0), 8, 6, facecolor="#e0f2fe", edgecolor="#334155"))
    triangle = [(1, 1), (7, 1), (4, 5)]
    centroid = (4, 7 / 3)
    sampled_pixel = (4, 2)
    ax.add_patch(Polygon(triangle, closed=True, facecolor="#7dd3fc", alpha=0.6, edgecolor="#0369a1"))
    ax.scatter(*centroid, s=60, color="#dc2626", zorder=3, label="triangle centroid")
    ax.scatter(*sampled_pixel, s=60, marker="s", color="#f59e0b", zorder=3, label="sampled pixel")
    ax.annotate("centroid g", xy=centroid, xytext=(5.1, 2.6), arrowprops={"arrowstyle": "->"})
    ax.annotate("nearest integer pixel", xy=sampled_pixel, xytext=(4.5, 1.0), arrowprops={"arrowstyle": "->"})
    ax.set_xticks(range(9))
    ax.set_yticks(range(7))
    ax.set_xlim(-0.5, 8.5)
    ax.set_ylim(-0.5, 6.5)
    ax.set_aspect("equal")
    ax.grid(True, alpha=0.25)
    ax.set_title("Centroid-based color sampling", weight="bold")
    ax.legend(loc="upper left", fontsize=8)
    fig.savefig(output, dpi=180, bbox_inches="tight")
    plt.close(fig)


def save_circumcircle(output: Path) -> None:
    fig, ax = plt.subplots(figsize=(6, 4.5), constrained_layout=True)
    triangle = [(1, 1), (7, 1), (4, 5)]
    center = (4, 2.25)
    radius = 2.75
    ax.add_patch(Circle(center, radius, fill=False, color="#7c3aed", linewidth=2, linestyle="--"))
    ax.add_patch(Polygon(triangle, closed=True, facecolor="#ddd6fe", edgecolor="#5b21b6", alpha=0.65))
    inside = (4, 2.0)
    outside = (7.2, 4.8)
    ax.scatter(*inside, color="#dc2626", s=55, label="point inside circumcircle")
    ax.scatter(*outside, color="#16a34a", s=55, label="point outside circumcircle")
    ax.scatter(*center, color="#7c3aed", s=35, label="circumcenter")
    for point, name in zip(triangle, ("a", "b", "c")):
        ax.scatter(*point, color="#0f172a", zorder=3)
        ax.text(point[0] + 0.12, point[1] + 0.12, name)
    style_axes(ax, "Circumcircle predicate", (0, 8), (0, 6))
    ax.legend(loc="lower right", fontsize=8)
    fig.savefig(output, dpi=180, bbox_inches="tight")
    plt.close(fig)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, default=Path("paper/figures/plots"))
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)

    save_orientation(args.output_dir / "orientation_examples.png")
    save_centroid(args.output_dir / "centroid_sampling.png")
    save_circumcircle(args.output_dir / "circumcircle_predicate.png")
    print(f"Wrote geometry figures to {args.output_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
