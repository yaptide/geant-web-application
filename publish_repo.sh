#!/bin/bash
set -e # exit script on any command fail

git config --local url."git@github.com:".insteadOf "https://github.com/"

pushd $MEMFS
        rm -rf geant-web-stubs
        git clone https://github.com/yaptide/geant-web-stubs

        pushd geant-web-application/build
            yes | cp -f geant4_wasm.js $MEMFS/geant-web-stubs/geant4_wasm.js
            yes | cp -f geant4_wasm.d.ts $MEMFS/geant-web-stubs/geant4_wasm.d.ts

            mkdir -p $MEMFS/geant-web-stubs/preload 
        	yes | cp -rf js/*.js $MEMFS/geant-web-stubs/preload 
        popd

        pushd geant-web-stubs
            git add .
            git commit -m "Stub update" # versioning
            git push
        popd
popd



