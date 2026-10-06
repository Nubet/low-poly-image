"""Visualize the main state changes of the project's Delaunay algorithm."""

from __future__ import annotations

import argparse
import math
from pathlib import Path

import matplotlib.pyplot as plt
from matplotlib.patches import Polygon

Point = tuple[float, float]
Triangle = tuple[int, int, int]
Edge = tuple[int, int]


def orientation(a: Point, b: Point, c: Point) -> float:
    return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])


def counterclockwise(triangle: Triangle, points: list[Point]) -> Triangle:
    a, b, c = triangle
    return (a, c, b) if orientation(points[a], points[b], points[c]) < 0 else triangle


def inside_circumcircle(a: Point, b: Point, c: Point, point: Point) -> bool:
    ax, ay = a[0] - point[0], a[1] - point[1]
    bx, by = b[0] - point[0], b[1] - point[1]
    cx, cy = c[0] - point[0], c[1] - point[1]
    determinant = (
        (ax * ax + ay * ay) * (bx * cy - cx * by)
        - (bx * bx + by * by) * (ax * cy - cx * ay)
        + (cx * cx + cy * cy) * (ax * by - bx * ay)
    )
    return determinant > 0


def toggle_edge(edges: list[Edge], edge: Edge) -> None:
    reverse = (edge[1], edge[0])
    if edge in edges:
        edges.remove(edge)
    elif reverse in edges:
        edges.remove(reverse)
    else:
        edges.append(edge)


def insert_point(
    points: list[Point], triangles: list[Triangle], point_index: int
) -> tuple[list[Triangle], list[Triangle], list[Edge]]:
    point = points[point_index]
    bad = [
        triangle
        for triangle in triangles
        if inside_circumcircle(
            points[triangle[0]], points[triangle[1]], points[triangle[2]], point
        )
    ]
    boundary: list[Edge] = []
    for triangle in bad:
        toggle_edge(boundary, (triangle[0], triangle[1]))
        toggle_edge(boundary, (triangle[1], triangle[2]))
        toggle_edge(boundary, (triangle[2], triangle[0]))

    remaining = [triangle for triangle in triangles if triangle not in bad]
    created = [counterclockwise((edge[0], edge[1], point_index), points) for edge in boundary]
    return remaining + created, bad, boundary


def draw_triangle(ax, triangle: Triangle, points: list[Point], **kwargs: object) -> None:
    ax.add_patch(Polygon([points[index] for index in triangle], closed=True, **kwargs))


def draw_mesh(ax, triangles: list[Triangle], points: list[Point], **kwargs: object) -> None:
    for triangle in triangles:
        draw_triangle(ax, triangle, points, **kwargs)


def setup_axes(ax, title: str) -> None:
    ax.set_title(title, fontsize=10, weight="bold")
    ax.set_xlim(-2, 12)
    ax.set_ylim(9, -3)
    ax.set_aspect("equal")
    ax.set_xticks(range(0, 11, 2))
    ax.set_yticks(range(0, 8, 2))
    ax.grid(True, alpha=0.2)


def draw_real_points(ax, points: list[Point], selected: int | None = None) -> None:
    real = points[:9]
    ax.scatter([point[0] for point in real], [point[1] for point in real], color="#0f172a", s=18, zorder=4)
    if selected is not None:
        point = points[selected]
        ax.scatter(*point, color="#dc2626", s=75, zorder=5, label="inserted point")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, default=Path("paper/figures/plots"))
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)

    real_points: list[Point] = [
        (0, 0),
        (10, 0),
        (10, 7),
        (0, 7),
        (3, 2),
        (7, 2),
        (5, 5),
        (2, 5),
        (8, 5),
    ]
    min_x = min(point[0] for point in real_points)
    max_x = max(point[0] for point in real_points)
    min_y = min(point[1] for point in real_points)
    max_y = max(point[1] for point in real_points)
    diameter = max(max_x - min_x, max_y - min_y)
    center = ((min_x + max_x) / 2, (min_y + max_y) / 2)
    points = real_points + [
        (center[0], center[1] - 2 * diameter),
        (center[0] - 2 * diameter, center[1] + 2 * diameter),
        (center[0] + 2 * diameter, center[1] + 2 * diameter),
    ]
    super_triangle = counterclockwise((9, 10, 11), points)
    triangles = [super_triangle]
    selected = 5

    before = triangles
    for point_index in range(selected):
        triangles, _, _ = insert_point(points, triangles, point_index)
    before = triangles
    after, bad, boundary = insert_point(points, before, selected)

    final_triangles = after
    for point_index in range(selected + 1, len(real_points)):
        final_triangles, _, _ = insert_point(points, final_triangles, point_index)
    final_triangles = [
        triangle
        for triangle in final_triangles
        if all(index < len(real_points) for index in triangle)
    ]

    fig, axes = plt.subplots(2, 3, figsize=(12, 7.5), constrained_layout=True)
    axes = axes.ravel()

    setup_axes(axes[0], "1. Initial super-triangle")
    axes[0].set_xlim(-18, 28)
    axes[0].set_ylim(26, -19)
    axes[0].set_xticks(range(-10, 30, 10))
    axes[0].set_yticks(range(-10, 30, 10))
    draw_mesh(axes[0], [super_triangle], points, facecolor="#ddd6fe", edgecolor="#7c3aed", alpha=0.55)
    draw_real_points(axes[0], points)

    setup_axes(axes[1], "2. Before inserting point")
    draw_mesh(axes[1], before, points, facecolor="#dbeafe", edgecolor="#2563eb", alpha=0.35)
    draw_real_points(axes[1], points, selected)

    setup_axes(axes[2], "3. Bad triangles")
    draw_mesh(axes[2], before, points, facecolor="#dbeafe", edgecolor="#94a3b8", alpha=0.2)
    draw_mesh(axes[2], bad, points, facecolor="#fecaca", edgecolor="#dc2626", alpha=0.75)
    draw_real_points(axes[2], points, selected)

    setup_axes(axes[3], "4. Cavity boundary")
    draw_mesh(axes[3], before, points, facecolor="#dbeafe", edgecolor="#cbd5e1", alpha=0.15)
    for edge in boundary:
        a, b = points[edge[0]], points[edge[1]]
        axes[3].plot([a[0], b[0]], [a[1], b[1]], color="#f97316", linewidth=3)
    draw_real_points(axes[3], points, selected)

    setup_axes(axes[4], "5. Retriangulated cavity")
    draw_mesh(axes[4], after, points, facecolor="#bfdbfe", edgecolor="#2563eb", alpha=0.5)
    draw_real_points(axes[4], points, selected)

    setup_axes(axes[5], "6. Final mesh")
    draw_mesh(axes[5], final_triangles, points, facecolor="#bae6fd", edgecolor="#0369a1", alpha=0.55)
    draw_real_points(axes[5], points)

    fig.suptitle("Bowyer-Watson-style incremental triangulation", fontsize=14, weight="bold")
    fig.savefig(args.output_dir / "bowyer_watson_steps.png", dpi=180, bbox_inches="tight")
    plt.close(fig)
    print(f"Wrote triangulation figure to {args.output_dir / 'bowyer_watson_steps.png'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
