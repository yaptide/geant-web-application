# Development / Building

This repository is primarily a **build + packaging** project for a Geant4 **WebAssembly** module.

There are **two build modes** wired in `CMakeLists.txt`:

1. **Emscripten/WASM build** (`EMSCRIPTEN` is set)
2. **Native build** (fallback; builds a library using a locally installed Geant4)

The provided shell scripts (`prepare.sh`, `compile_application.sh`, ...) are tailored for an
environment that provides **Environment Modules** (`module add ...`) and a fast scratch workspace
via `MEMFS`.

If you are building locally (without `module`), treat the scripts as *reference* and adapt them to
your environment (install the required tools yourself, and remove the `module add ...` lines).

## Prerequisites

### Required for WASM builds

* CMake >= 3.22
* Python 3
* Node.js + npm (for the `typescript` package installed by `prepare.sh`)
* Emscripten SDK (emsdk) – scripts pin **4.0.13**
* Geant4 built for Emscripten (scripts build Geant4 **11.3.2**)
* Expat **2.6.4** (built for Emscripten)
* Xerces-C **3.3.0** (built for Emscripten)

### Required for publishing

* rclone
* s3cmd

## Building with the provided scripts

### 0) Set a build workspace (`MEMFS`)

The scripts expect a writable workspace:

```bash
export MEMFS=/path/to/fast/scratch
```

### 1) Bootstrap toolchain + datasets

```bash
./prepare.sh
```

This will download and build:

* emsdk (4.0.13)
* expat (2.6.4)
* xerces-c (3.3.0)
* geant4 (11.3.2)

and stage Geant4 datasets under:

* `$MEMFS/sources/geant4/datasets/`

### 2) Compile the WASM module

```bash
./compile_application.sh
```

This performs an Emscripten CMake configure + build, and then stages artifacts into:

* `$MEMFS/bucket-staging/geant4-wasm/`

including `geant4_wasm.wasm`, dataset payloads, and lazy-loading metadata.

## Building manually (Emscripten)

If you already have Emscripten + all deps installed, the core build looks like:

```bash
emcmake cmake -S . -B build \
  -DEXPAT_INCLUDE_DIR=... \
  -DEXPAT_LIBRARY=... \
  -DXercesC_INCLUDE_DIR=... \
  -DXercesC_LIBRARY=... \
  -DGeant4_DIR=... \
  -DPTL_DIR=... \
  -DDATASETS_SOURCE_DIR=... \
  -DDATASETS_LIST=G4EMLOW8.6.1;PhotonEvaporation6.1;G4NDL4.7.1;G4SAIDDATA2.0;G4ENSDFSTATE3.0;G4PARTICLEXS4.1 \
  -DLAZY_GENERATOR=$PWD/utils/lazy_json_generator.py \
  -DEMSCRIPTEN_FILE_PACKAGER=/path/to/emscripten/tools/file_packager.py

emmake cmake --build build -j
```

## Native build

The non‑Emscripten branch in `CMakeLists.txt` builds a **library** (not an executable) and expects a
native Geant4 installation.

By default it looks for Geant4 under:

* `/workspaces/cpp/geant4/install`

You can override by configuring `Geant4_DIR` to your own install:

```bash
cmake -S . -B build-native -DGeant4_DIR=/path/to/geant4/lib/cmake/Geant4
cmake --build build-native
```

## Coding conventions

* C++17 (`set(CMAKE_CXX_STANDARD 17)`)
* Keep the WASM-facing API minimal (see `geant4.hpp`) and keep simulation logic inside `include/` + `src/`.
