# Runtime API (JavaScript)

The WASM build exposes a small embind-based API declared in `geant4.hpp`.

## Module entrypoint

The Emscripten JS glue exports a factory function named:

* `createWasmModule`

It returns a Promise that resolves to the instantiated module.

```js
import createWasmModule from "./geant4_wasm.js";

const mod = await createWasmModule({
  locateFile: (p) => new URL(p, self.location.href).toString(),
});
```

> The build targets a **Web Worker** environment (`-s ENVIRONMENT='worker'`).

## Exported functions

### `Geant4SetProgressFunction(functionPointer: number): void`

Registers a callback that will be invoked periodically during the simulation.

* The callback receives the current `eventId` (integer).
* The callback is called every **100** events (see `src/EventAction.cc`).

Typical usage:

```js
// Signature: (int) -> void
const cbPtr = mod.addFunction((eventId) => {
  console.log("progress", eventId);
}, "vi");

mod.Geant4SetProgressFunction(cbPtr);
```

**Notes**

* `addFunction` is made available because the build exports `EXPORTED_RUNTIME_METHODS=["FS","addFunction"]`.
* In Emscripten, you are responsible for managing the lifetime of function pointers (e.g. via `removeFunction`) in long-lived apps.

### `Geant4GDMRun(gdmlPath: string, macroPath: string): number`

Runs a Geant4 session based on:

* a **GDML** geometry file (`gdmlPath`)
* a Geant4 **macro** file (`macroPath`) executed with `/control/execute`

Inputs are read from the Emscripten virtual filesystem (MEMFS).

Example:

```js
mod.FS.writeFile("/geometry.gdml", gdmlText);
mod.FS.writeFile("/run.mac", macroText);

const rc = mod.Geant4GDMRun("/geometry.gdml", "/run.mac");
if (rc !== 0) throw new Error(`Geant4GDMRun failed: ${rc}`);
```

### Output files

The C++ code opens an analysis file with:

* base name: `output`
* default type: `root`

In practice this produces:

* `output.root` in the module working directory

You can read it back via Emscripten FS:

```js
const bytes = mod.FS.readFile("output.root");
```

## Filesystem and datasets

Geant4 dataset locations are configured through environment variables set in `scripts/setup_env.js`, pointing to `/data/<dataset>`.

Make sure those directories exist in the Emscripten filesystem (either via preloading `.data` payloads or via your own lazy-loading implementation).
