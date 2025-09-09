#!/usr/bin/env bash

# Path to your .env file (can be passed as first argument)
ENV_FILE="${1:-.env}"

# Ensure .env exists
if [[ ! -f "$ENV_FILE" ]]; then
  echo "Error: $ENV_FILE not found."
  exit 1
fi

# Read each line, skip comments and blanks, and export RCLONE_CONFIG_* variables
while IFS='=' read -r key value; do
  # Skip empty lines or comments
  [[ -z "$key" || "$key" == \#* ]] && continue

  # Only process RCLONE_CONFIG_* variables
  if [[ "$key" == RCLONE_CONFIG_* ]]; then
    # Remove surrounding quotes if any
    value="${value%\"}"
    value="${value#\"}"

    export "$key=$value"
  fi
done < "$ENV_FILE"

echo "[default]
access_key = ${RCLONE_CONFIG_GEANT4_WEB_ACCESS_KEY_ID}
secret_key = ${RCLONE_CONFIG_GEANT4_WEB_SECRET_ACCESS_KEY}
bucket_location = us-east-1
check_ssl_certificate = True
check_ssl_hostname = True
enable_multipart = True
host_base = ${RCLONE_CONFIG_GEANT4_WEB_ENDPOINT}
host_bucket = geant4-wasm.${RCLONE_CONFIG_GEANT4_WEB_ENDPOINT}
multipart_chunk_size_mb = 256
use_https = True
signurl_use_https = True" > .s3cfg
