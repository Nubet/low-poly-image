# Data Flow

The same processing pipeline is used by the CLI and the GUI.

```text
Input path
    |
    v
Load image
    |
    v
Generate points and image corners
    |
    v
Build Delaunay triangulation
    |
    v
Sample one color per triangle
    |
    v
Rasterize triangles
    |
    v
Output image
```

## CLI

The CLI parses the input path, output path, point count, and optional seed.
`lowpoly_process()` loads the image and produces an in-memory result. The CLI
then saves the result to the requested output path.

## GUI

The GUI collects the same values through Slint. Processing runs on a worker
thread so the window remains responsive. When processing finishes, the result
is sent back to the Slint event loop and displayed in the preview.

Saving uses the last completed response. The output format is selected from the
file extension: `.jpg` and `.jpeg` use JPEG; other output paths use PNG.

## Ownership

The C arena owns temporary processing data. The C++ service copies the final
image into an owned response object. The GUI keeps that response alive while
the user previews or saves the result.
