# Geant4 WebAssembly module (geant-web-application)

This repository builds a **Geant4** simulation runtime into a **WebAssembly (WASM)** module intended to run in a **Web Worker** (browser-side) and be distributed together with the required **Geant4 datasets**.

Besides the C++ sources, the repository includes scripts that:

* bootstrap a reproducible Emscripten toolchain + dependencies (Expat, Xerces-C, Geant4)
* package Geant4 datasets for Emscripten’s virtual filesystem
* publish build artifacts and datasets to **Cyfronet S3 / Ceph**
* update a separate “stubs” repository with the generated JS glue code and TypeScript typings

> **Audience:** This repo is primarily a *build/artifact* repository (WASM module + datasets), not a full web UI.

## What gets produced

When built for Emscripten, the output includes:

* `geant4_wasm.wasm` – the WebAssembly module
* `geant4_wasm.js` – Emscripten JS loader (ES module export name: `createWasmModule`)
* `geant4_wasm.d.ts` – generated TypeScript typings
* dataset payloads and preload scripts under `build/data/` and `build/js/`
* optional “lazy loading” metadata JSON files under `build/lazy_files/`

## Repository layout

* `geant4.cpp / geant4.hpp` – WASM-facing API surface (Emscripten embind)
* `include/` + `src/` – Geant4 user classes (physics list, generator, actions)
* `scripts/setup_env.js` – injected with `--pre-js` to set Geant4 dataset env vars in the WASM runtime
* `utils/lazy_json_generator.py` – generates JSON manifests used for lazy dataset loading
* `prepare.sh` – downloads & builds the toolchain/deps and stages datasets
* `compile_application.sh` – builds the WASM module and stages artifacts
* `publish.sh` – uploads staged artifacts to S3 and configures CORS/policy
* `publish_repo.sh` – copies generated stubs (JS + d.ts + preload scripts) into `geant-web-stubs`

## Prerequisites

### For the provided scripts

The scripts assume an environment with:

* **Environment Modules** (`module add ...`) providing at least `cmake` and `rclone`
* `wget`, `tar`, `git`, `npm`, `python`
* outbound network access to download Geant4 datasets and sources
* a writable staging area referenced by `MEMFS`

> If you’re *not* on an HPC environment that provides `module`, you can still build locally, but you’ll need to adapt the scripts (see `docs/DEVELOPMENT.md`).

### Versions pinned by `prepare.sh`

* Emscripten SDK: **4.0.13**
* Geant4: **11.3.2**
* Expat: **2.6.4**
* Xerces-C: **3.3.0**
* Geant4 datasets (subset staged by default):
  * `G4NDL.4.7.1` → `G4NDL4.7.1`
  * `G4EMLOW.8.6.1` → `G4EMLOW8.6.1`
  * `G4PhotonEvaporation.6.1` → `PhotonEvaporation6.1`
  * `G4ENSDFSTATE.3.0` → `G4ENSDFSTATE3.0`
  * `G4SAIDDATA.2.0` → `G4SAIDDATA2.0`
  * `G4PARTICLEXS.4.1` → `G4PARTICLEXS4.1`

## Quickstart (scripted build & publish)

The happy path (as intended by this repo) is:

```bash
./prepare.sh
./compile_application.sh
./publish.sh
./publish_repo.sh
```

### Required environment variables

The scripts rely on `MEMFS` pointing to a writable working directory used for downloads, builds, and staging.

Example:

```bash
export MEMFS=/path/to/fast/scratch
```

### S3 credentials (.env)

`publish.sh` reads S3 credentials through `setup_env.sh` which expects a `.env` file containing `RCLONE_CONFIG_*` variables.

Template:

```env
RCLONE_CONFIG_GEANT4_WEB_TYPE=s3
RCLONE_CONFIG_GEANT4_WEB_PROVIDER=Ceph
RCLONE_CONFIG_GEANT4_WEB_ACCESS_KEY_ID=<access_key_id>
RCLONE_CONFIG_GEANT4_WEB_SECRET_ACCESS_KEY=<secret_key>
RCLONE_CONFIG_GEANT4_WEB_ENDPOINT=https://s3p.cloud.cyfronet.pl
RCLONE_CONFIG_GEANT4_WEB_REGION=
RCLONE_CONFIG_GEANT4_WEB_LOCATION_CONSTRAINT=
RCLONE_CONFIG_GEANT4_WEB_ACL=private
```

> `setup_env.sh` also generates a local `.s3cfg` used by `s3cmd` for setting bucket CORS and policy.

## Using the WASM module from JavaScript

The module is compiled with:

* ES module export name: **`createWasmModule`**
* environment: **`worker`**
* runtime methods exported: **`FS`** and **`addFunction`**

High-level usage pattern:

1. Instantiate the module (`await createWasmModule(...)`).
2. Write your input files (GDML + macro) into Emscripten FS.
3. Optionally register a progress callback.
4. Call `Geant4GDMRun(gdmlPath, macroPath)`.
5. Read the resulting `output.root` from FS.

Example (simplified):

```js
import createWasmModule from "./geant4_wasm.js";

const mod = await createWasmModule({
  // Adjust if you host .wasm/.data files on a CDN
  locateFile: (p) => new URL(p, self.location.href).toString(),
});

// Progress callback (called every 100 events)
const cbPtr = mod.addFunction((eventId) => {
  console.log("progress event", eventId);
}, "vi");
mod.Geant4SetProgressFunction(cbPtr);

// Provide inputs
mod.FS.writeFile("/geometry.gdml", gdmlText);
mod.FS.writeFile("/run.mac", macroText);

// Run
mod.Geant4GDMRun("/geometry.gdml", "/run.mac");

// Retrieve output produced by G4AnalysisManager
const rootBytes = mod.FS.readFile("output.root");
```

> Geant4 dataset environment variables (e.g. `G4LEDATA`, `G4NEUTRONHPDATA`, …) are set by `scripts/setup_env.js` at startup.

## Documentation

* `docs/DEVELOPMENT.md` – building locally vs in an HPC/modules environment
* `docs/ARCHITECTURE.md` – how the WASM build is wired (CMake + Emscripten + datasets)
* `docs/API.md` – runtime API exposed to JS (inputs/outputs)
* `docs/DEPLOYMENT.md` – publishing to S3 and updating stub artifacts
* `docs/TROUBLESHOOTING.md` – common build/runtime issues

## Contributing

See `docs/DEVELOPMENT.md` for build conventions and tips.
