# Troubleshooting

## Build configuration errors

### “You must specify EXPAT_INCLUDE_DIR / EXPAT_LIBRARY / XercesC_INCLUDE_DIR / XercesC_LIBRARY”

These are hard requirements in `CMakeLists.txt`.

If you’re using the provided scripts, the configure step is expected to pass values like:

* `-DEXPAT_INCLUDE_DIR=$MEMFS/sources/expat-2.6.4/install/include`
* `-DEXPAT_LIBRARY=$MEMFS/sources/expat-2.6.4/install/lib/libexpat.a`

If you’re configuring manually, ensure you point to **Emscripten-built** static libraries (not system ones).

### “You must specify Geant4_DIR / PTL_DIR / ...”

These are required only in the Emscripten build branch.

They should resolve to your Emscripten-built Geant4 installation, for example:

* `-DGeant4_DIR=$MEMFS/sources/geant4/install/lib/cmake/Geant4`
* `-DPTL_DIR=$MEMFS/sources/geant4/install/lib/cmake/Geant4/PTL`

### Emscripten tools not found (`emcmake`, `emmake`)

Make sure you sourced the emsdk environment:

```bash
source /path/to/emsdk/emsdk_env.sh
```

The scripts do this implicitly from `$MEMFS/sources/emsdk-4.0.13/emsdk_env.sh`.

## Runtime issues

### Missing datasets / Geant4 complains about data paths

Symptoms can include warnings/errors about missing cross section tables or physics data.

Checklist:

1. Ensure `scripts/setup_env.js` is being injected (it is referenced in `CMakeLists.txt` via `--pre-js ./setup_env.js`).
2. Ensure the `/data/<dataset>` directories exist in the Emscripten filesystem.
   * If using `.data` preloads: make sure preload scripts are executed before running.
   * If using lazy loading: ensure your runtime mounts/creates the directories and fetches files on demand.

### `Geant4SetProgressFunction` never fires

* Progress is emitted only every **100** events (`eventId % 100 == 0`).
* The callback is invoked from `EventAction::BeginOfEventAction`.
* Ensure you registered a callback pointer via `addFunction` and called `Geant4SetProgressFunction` before the run.

### `output.root` is missing

The code opens the analysis output with base name `output`.

Confirm that:

* your macro actually starts a run (`/run/beamOn ...`)
* the Emscripten filesystem is writable
* you read the correct filename (`output.root`)

## Deployment issues

### `rclone` remote not found (`geant4_web:`)

`publish.sh` expects the rclone remote to be configured via environment variables loaded from `.env`.

Make sure:

* `.env` exists in the repo root (or you pass the path to `setup_env.sh`)
* variables are named exactly `RCLONE_CONFIG_GEANT4_WEB_*`

### CORS errors in the browser

If the browser cannot fetch `.wasm` or dataset files due to CORS:

* re-run `publish.sh` to apply `cors.xml`
* confirm you are accessing the correct bucket/endpoint

### Public access denied

If clients cannot read objects:

* confirm `policy.json` has been applied
* confirm object ACLs and bucket policy align (note: `.env` sets `RCLONE_CONFIG_GEANT4_WEB_ACL=private` by default)

Depending on your Ceph/S3 configuration, you may need to adjust ACL or bucket policy strategy.
