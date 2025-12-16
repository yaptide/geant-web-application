# Architecture

At a high level, this project wraps a minimal Geant4 application and compiles it to **WebAssembly** via **Emscripten**.

The output is designed to run in a **Web Worker** and exposes a small JavaScript-callable API via **embind**.

## Key components

### 1) WASM-facing API surface

The public interface is defined in `geant4.hpp` and exported via embind when building with Emscripten:

* `Geant4SetProgressFunction(functionPointer)`
* `Geant4GDMRun(gdml_file, macro_file)`

`Geant4SetProgressFunction` stores a function pointer and the code calls it from the `EventAction` every 100 events.

### 2) Geant4 user classes

The Geant4 “application” itself is mostly standard:

* **Physics list:** `PhysicsList` derives from `QBBC` (see `include/PhysicsList.hh`).
* **Geometry:** loaded from GDML (`G4GDMLParser`) in `Geant4GDMRun`.
* **Primary generation:** uses `G4GeneralParticleSource`.
* **Actions:** `RunAction` writes an analysis file at the end of the run; `EventAction` reports progress.

### 3) Dataset wiring

Geant4 uses environment variables to locate datasets (e.g. `G4LEDATA`, `G4NEUTRONHPDATA`).

This repository injects `scripts/setup_env.js` via the Emscripten linker flag `--pre-js`.
That script sets the necessary environment variables to point at dataset directories under `/data/...`.

### 4) Dataset packaging (Emscripten FS)

For each dataset listed in `DATASETS_LIST`, the build runs Emscripten’s `file_packager.py` after linking:

* produces a `data/<dataset>.data` payload
* produces a `js/preload_<dataset>.js` preload script
* produces a `.metadata` file (because `--separate-metadata` is enabled)

This is implemented in the `foreach(dataset ${DATASETS_LIST})` blocks inside `CMakeLists.txt`.

### 5) Lazy-loading metadata

In addition to preloading, the build invokes `utils/lazy_json_generator.py` to generate a JSON manifest for each dataset:

* output location: `build/lazy_files/load_<dataset>.json`
* virtual path prefix: `/data/<dataset>`
* URL prefix: `https://s3p.cloud.cyfronet.pl/geant4-wasm/datasets/<dataset>`

The intent is to allow a front-end runtime to lazily fetch individual files rather than always downloading full `.data` payloads.

## Build outputs & staging

The script `compile_application.sh` copies key build artifacts into:

* `$MEMFS/bucket-staging/geant4-wasm/`

with the following layout:

* `geant4_wasm.wasm`
* `datafiles/` – `.data` files and `.metadata`
* `lazy_files_metadata/` – JSON manifests from `lazy_json_generator.py`
* `datasets/` – raw dataset directories staged by `prepare.sh`

## Emscripten linker configuration

The WASM target is built with notable settings in `CMakeLists.txt`:

* `-s MODULARIZE=1` and `-s EXPORT_ES6=1` (ES module output)
* `-s EXPORT_NAME=createWasmModule` (entrypoint name)
* `-s ENVIRONMENT='worker'` (web worker environment)
* `-s FORCE_FILESYSTEM` and exported runtime methods `FS` + `addFunction`
* `--use-preload-cache` (enables caching for preloaded files)
* `--emit-tsd geant4_wasm.d.ts` (TypeScript definition generation)

If you change any of the above, ensure downstream consumers (e.g. `geant-web-stubs`) are updated.
