# Build System

The project uses CMake to build the C core, C++ service, CLI, tests, and
optional GUI.

## Targets

- `lowpoly_core`: C image and geometry implementation.
- `lowpoly_application`: C++ service wrapper.
- `lowpoly`: command-line application.
- `lowpoly_tests`: GoogleTest suite.
- `lowpoly_gui`: Slint desktop application when GUI support is enabled.

## Languages

- C17 is used by the processing core and CLI.
- C++20 is used by the service and GUI.
- Rust is required by Slint during GUI compilation.

## Dependencies

CMake downloads pinned dependencies with `FetchContent`:

- GoogleTest for tests,
- Slint for the GUI,
- nativefiledialog-extended for native file dialogs.

Image loading and writing use pinned `stb` headers downloaded during CMake
configuration.

## GUI Build

The GUI is disabled by default. Enable it with:

```sh
cmake -S . -B build-gui -DLOWPOLY_BUILD_GUI=ON
cmake --build build-gui --target lowpoly_gui
```

Linux also needs GTK3 and Fontconfig development packages. Windows uses MSVC;
the project maps warning flags to `/W4` for that compiler.
