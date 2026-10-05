# GUI

The GUI is built with Slint and is optional. It is enabled with
`LOWPOLY_BUILD_GUI=ON`.

## UI Responsibilities

`ui/main.slint` owns the visual state:

- input path,
- point count,
- seed and fixed-seed mode,
- processing state,
- original image preview,
- generated image preview,
- status text.

The Slint file does not run the image algorithm.

## C++ Responsibilities

`src/gui/main.cpp` connects UI events to the application layer:

- native file dialogs come from nativefiledialog-extended,
- input values are validated before processing,
- a random seed is generated when fixed-seed mode is disabled,
- `std::jthread` runs processing and saving away from the UI thread,
- `slint::invoke_from_event_loop()` applies results to the UI safely.

The original image is loaded for preview, while the generated image comes from
the C++ service response. The user can save the completed response as PNG or
JPEG.

## Why the Worker Thread Exists

Image loading, triangulation, and rasterization can take noticeable time. They
must not block the Slint event loop, otherwise the window would stop repainting
and responding to input during processing.
