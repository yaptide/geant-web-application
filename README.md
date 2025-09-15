# geant-web-application
Repository holding main Geant4 WebAssembly module. It also contains compilation scripts with helpers used in deployment of compiled binaries and Geant4 datasets at Cyfronet S3 object storage.

# .env
`.env` file template
```
RCLONE_CONFIG_GEANT4_WEB_TYPE=s3
RCLONE_CONFIG_GEANT4_WEB_PROVIDER=Ceph
RCLONE_CONFIG_GEANT4_WEB_ACCESS_KEY_ID=<access_key_id>
RCLONE_CONFIG_GEANT4_WEB_SECRET_ACCESS_KEY=<secret_key>
RCLONE_CONFIG_GEANT4_WEB_ENDPOINT=https://s3p.cloud.cyfronet.pl
RCLONE_CONFIG_GEANT4_WEB_REGION=
RCLONE_CONFIG_GEANT4_WEB_LOCATION_CONSTRAINT=
RCLONE_CONFIG_GEANT4_WEB_ACL=private
```

```
./prepare.sh
./compile_application.sh
./publish.sh
./publish_repo.sh
```