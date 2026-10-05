# Algorithm

The generator creates a low-poly image by replacing the image with colored
triangles.

## 1. Load the Image

The input is loaded into an RGB image buffer. Invalid paths, unsupported input,
and invalid point counts stop the pipeline before allocation-heavy work starts.

## 2. Generate Points

The requested point set contains:

- the four image corners,
- random points inside the image bounds.

The seed controls the random sequence. The same input, point count, and seed
produce the same point set.

Keeping the four corners guarantees that the generated mesh covers the image
area.

## 3. Build the Delaunay Mesh

The implementation uses an incremental Bowyer-Watson-style process:

1. Create a large temporary super-triangle around the image points.
2. Insert the real points one by one.
3. Find triangles whose circumcircle contains the new point.
4. Remove those triangles.
5. Keep the boundary edges that occur only once.
6. Create new triangles from the boundary edges and the new point.
7. Remove triangles connected to the temporary super-triangle.

The result is a set of non-degenerate triangles with Delaunay-like spacing.

## 4. Assign Triangle Colors

For each triangle, the renderer calculates its centroid and samples the input
pixel at that position. That one RGB value becomes the fill color for the whole
triangle.

## 5. Rasterize the Result

Each triangle is drawn inside its bounding box. Pixel centers are tested against
the triangle, and covered pixels receive the sampled color.

More points create smaller triangles and preserve more detail. Fewer points
create larger shapes and a stronger low-poly effect.

## Main Trade-offs

- More points improve detail but increase triangulation and rendering time.
- A fixed seed makes results reproducible.
- Centroid sampling is fast, but it does not average all source pixels inside a
  triangle.
- The algorithm is deterministic after the seed is chosen.
