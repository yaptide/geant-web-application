#!/bin/bash
set -e

module add rclone
python -m pip install s3cmd

source setup_env.sh

rclone mkdir geant4_web:geant4-wasm
rclone sync $MEMFS/bucket-staging/geant4-wasm geant4_web:geant4-wasm -P

s3cmd -c .s3cfg setcors cors.xml s3://geant4-wasm 
s3cmd -c .s3cfg setpolicy policy.json s3://geant4-wasm 
