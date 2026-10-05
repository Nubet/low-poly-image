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

The test target uses GoogleTest 1.17.0 and is downloaded by CMake during
configuration.

## Tests

```sh
ctest --test-dir build --output-on-failure
```

## Run

```sh
cmake --build . --target run
```

Uses all supported images from `test_images/in`, generates 500 points, and
writes the results to `test_images/out`.

## Usage

The simplest form generates `input_lowpoly.png` next to the input file:

```sh
lowpoly test_images/in/10136-00.jpg
```

The default seed is random and is printed by the program. Use that value with
`--seed` to reproduce the result.

Options:

```text
--output path    output PNG path
--points count   number of points, from 4 to 1000000 (default: 500)
--seed value     deterministic unsigned seed
--help           print usage
```

Windows:

```powershell
lowpoly.exe test_images\in\10136-00.jpg --output output.png --points 500 --seed 123
```

Linux:

```sh
./lowpoly test_images/in/10136-00.jpg --output output.png --points 500 --seed 123
```

The input image may be read by `stb_image`; the output is always PNG. Images
larger than 100 million pixels are rejected before processing.

## Image Dataset

Repository test images are stored in `test_images/in`. Generate low-poly
results for all supported images into `test_images/out` with:

```sh
cmake --build build --target run
```

The batch target uses 500 points and seed `123`, so the generated results are
repeatable.
