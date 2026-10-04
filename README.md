# Lowpoly

## Requirements

- C compiler
- CMake

## Build

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

## Run

```sh
cmake --build . --target run
```

Uses `../input.png` and writes `output.png`.

## Custom Input

Windows:

```powershell
lowpoly.exe ..\input.png output.png 500
```

Linux:

```sh
./lowpoly ../input.png output.png 500
```
