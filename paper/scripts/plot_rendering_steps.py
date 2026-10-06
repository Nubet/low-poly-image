"""Visualize centroid sampling and triangle rasterization."""

from __future__ import annotations

import argparse
from pathlib import Path

import matplotlib.pyplot as plt
from matplotlib.patches import Polygon, Rectangle


TRIANGLE = [(1.5, 1.5), (7.0, 2.0), (3.5, 5.2)]
WIDTH = 9
HEIGHT = 7


def orientation(a: tuple[float, float], b: tuple[float, float], c: tuple[float, float]) -> float:
    return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])


def inside(point: tuple[float, float]) -> bool:
    signs = [
        orientation(TRIANGLE[0], TRIANGLE[1], point),
        orientation(TRIANGLE[1], TRIANGLE[2], point),
        orientation(TRIANGLE[2], TRIANGLE[0], point),
    ]
    return all(sign >= 0 for sign in signs) or all(sign <= 0 for sign in signs)


def image_data() -> list[list[tuple[float, float, float]]]:
    return [
        [(0.18 + 0.07 * x, 0.35 + 0.04 * y, 0.75 - 0.04 * x) for x in range(WIDTH)]
        for y in range(HEIGHT)
    ]


def draw_frame(ax, title: str) -> None:
    ax.set_title(title, fontsize=10, weight="bold")
    ax.set_xlim(0, WIDTH)
    ax.set_ylim(HEIGHT, 0)
    ax.set_aspect("equal")
    ax.set_xticks(range(WIDTH + 1))
    ax.set_yticks(range(HEIGHT + 1))
    ax.grid(True, color="#64748b", alpha=0.3)


def draw_image(ax) -> None:
    ax.imshow(image_data(), extent=(0, WIDTH, HEIGHT, 0), interpolation="nearest")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, default=Path("paper/figures/plots"))
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)

    centroid = tuple(sum(point[index] for point in TRIANGLE) / 3 for index in (0, 1))
    sample_pixel = (int(centroid[0]), int(centroid[1]))
    min_x = int(min(point[0] for point in TRIANGLE))
    max_x = int(max(point[0] for point in TRIANGLE))
    min_y = int(min(point[1] for point in TRIANGLE))
    max_y = int(max(point[1] for point in TRIANGLE))

    fig, axes = plt.subplots(2, 2, figsize=(9, 8), constrained_layout=True)
    axes = axes.ravel()

    draw_frame(axes[0], "1. Input pixels and triangle")
    draw_image(axes[0])
    axes[0].add_patch(Polygon(TRIANGLE, closed=True, fill=False, edgecolor="#dc2626", linewidth=2))

    draw_frame(axes[1], "2. Centroid sample")
    draw_image(axes[1])
    axes[1].add_patch(Polygon(TRIANGLE, closed=True, fill=False, edgecolor="#dc2626", linewidth=2))
    axes[1].scatter(*centroid, color="#dc2626", s=65, zorder=4)
    axes[1].scatter(*sample_pixel, marker="s", color="#f59e0b", s=70, zorder=4)
    axes[1].annotate("centroid", xy=centroid, xytext=(5.8, 1.1), arrowprops={"arrowstyle": "->"})
    axes[1].annotate("sampled pixel", xy=sample_pixel, xytext=(5.4, 5.8), arrowprops={"arrowstyle": "->"})

    draw_frame(axes[2], "3. Bounding-box pixel tests")
    draw_image(axes[2])
    axes[2].add_patch(
        Rectangle((min_x, min_y), max_x - min_x + 1, max_y - min_y + 1, fill=False, edgecolor="#f97316", linewidth=2)
    )
    for y in range(min_y, max_y + 1):
        for x in range(min_x, max_x + 1):
            color = "#16a34a" if inside((x + 0.5, y + 0.5)) else "#64748b"
            axes[2].scatter(x + 0.5, y + 0.5, color=color, s=12, zorder=4)

    draw_frame(axes[3], "4. Filled triangle output")
    draw_image(axes[3])
    fill_color = image_data()[sample_pixel[1]][sample_pixel[0]]
    axes[3].add_patch(Polygon(TRIANGLE, closed=True, facecolor=fill_color, edgecolor="#0f172a", alpha=0.9))
    axes[3].scatter(*centroid, color="#f59e0b", s=55, zorder=4)

    fig.suptitle("From a geometric triangle to raster pixels", fontsize=14, weight="bold")
    fig.savefig(args.output_dir / "rendering_steps.png", dpi=180, bbox_inches="tight")
    plt.close(fig)
    print(f"Wrote rendering figure to {args.output_dir / 'rendering_steps.png'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
