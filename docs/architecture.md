# Architecture

Lowpoly Image Generator is split into a small C processing core and thin C++
integration layers.

```text
CLI / Slint GUI
       |
       v
lowpoly_application
       |
       v
lowpoly_core
       |
       v
image -> points -> triangulation -> rendering
```

## C Core

The C core owns the image-processing pipeline:

- `image.c` loads and saves image data.
- `random.c` creates deterministic random sequences.
- `geometry.c` provides orientation and triangle helpers.
- `triangulation.c` creates a Delaunay mesh.
- `renderer.c` fills triangles with sampled colors.
- `processing.c` coordinates the complete operation.

The core uses an arena allocator for short-lived processing data. The main
processing function is `lowpoly_process()` in `src/processing.c`.

## C++ Layer

`lowpoly_service.cpp` wraps the C API in a small C++ service. It owns the
response image and exposes a simple request/response interface to the GUI.

The C++ layer does not implement the image algorithm. It adapts data ownership,
formats, and error statuses for the application layer.

## Application Interfaces

- `src/main.c` provides the command-line interface.
- `src/gui/main.cpp` connects Slint callbacks to the C++ service.
- `ui/main.slint` defines the GUI layout and UI state.

This keeps UI concerns out of the image-processing core and allows the CLI,
tests, and GUI to use the same algorithm.
