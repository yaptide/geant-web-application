# Deployment (Cyfronet S3 / Ceph)

This repository’s deployment flow is centered around uploading build artifacts and datasets to an S3-compatible object store (Cyfronet S3 / Ceph).

The scripted flow is:

```bash
./prepare.sh
./compile_application.sh
./publish.sh
./publish_repo.sh
```

## 1) Staging location

Both `prepare.sh` and `compile_application.sh` use `MEMFS` as the root workspace.

Artifacts intended for publishing are staged under:

* `$MEMFS/bucket-staging/geant4-wasm/`

## 2) Credentials and configuration

### `.env`

Publishing uses `rclone` with an S3 remote named `geant4_web`. The remote configuration is provided through environment variables loaded from `.env`.

Create a `.env` file (or pass a different filename to `setup_env.sh`) with:

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

### `setup_env.sh`

`publish.sh` sources `setup_env.sh`, which:

* exports `RCLONE_CONFIG_*` variables into the current shell
* generates a local `.s3cfg` file for `s3cmd`

## 3) Uploading artifacts

`publish.sh` performs:

1. `rclone mkdir geant4_web:geant4-wasm`
2. `rclone sync $MEMFS/bucket-staging/geant4-wasm geant4_web:geant4-wasm -P`

This uploads the WASM module and related files into the bucket:

* `geant4-wasm`

## 4) Bucket CORS and policy

After syncing, `publish.sh` applies:

* `cors.xml` – allows `GET` and `HEAD` from any origin
* `policy.json` – allows public `s3:GetObject` on `arn:aws:s3:::geant4-wasm/*`

These are applied using `s3cmd`:

```bash
s3cmd -c .s3cfg setcors cors.xml s3://geant4-wasm
s3cmd -c .s3cfg setpolicy policy.json s3://geant4-wasm
```

## 5) Updating the JS/TypeScript stubs repository

`publish_repo.sh` updates a separate repository:

* `https://github.com/yaptide/geant-web-stubs`

It copies:

* `build/geant4_wasm.js`
* `build/geant4_wasm.d.ts`
* `build/js/*.js` preload scripts → `geant-web-stubs/preload/`

Then it commits and pushes.

### Authentication

`publish_repo.sh` rewrites GitHub HTTPS URLs to SSH (`git@github.com:`). Ensure your environment has an SSH key configured for GitHub.

## Rollback strategy

Because artifacts are uploaded to a bucket, the simplest rollback is to:

* re-upload a previous known-good build (or restore from bucket versioning if enabled)
* revert the commit in `geant-web-stubs` (or tag builds/releases)

If this project is used in production, consider:

* enabling bucket versioning
* publishing immutable build paths (e.g. `/releases/<version>/...`) and updating a small “latest” pointer