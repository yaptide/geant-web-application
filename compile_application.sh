#!/bin/bash

module add cmake/3.24.3
PATH=$PATH:~/.npm_global/bin

pushd $MEMFS
    mkdir -p $MEMFS/geant-web-application
    yes | cp -rfa $HOME/geant-web-application/* $MEMFS/geant-web-application

    source $MEMFS/sources/emsdk-4.0.13/emsdk_env.sh

    pushd geant-web-application
        emcmake cmake -S . -B build \
            -DEXPAT_INCLUDE_DIR=$MEMFS/sources/expat-2.6.4/install/include \
            -DEXPAT_LIBRARY=$MEMFS/sources/expat-2.6.4/install/lib/libexpat.a \
            -DXercesC_INCLUDE_DIR=$MEMFS/sources/xerces-c-3.3.0/install/include \
            -DXercesC_LIBRARY=$MEMFS/sources/xerces-c-3.3.0/install/lib/libxerces-c-3.3.a \
            -DGeant4_DIR=$MEMFS/sources/geant4/install/lib/cmake/Geant4 \
            -DPTL_DIR=$MEMFS/sources/geant4/install/lib/cmake/Geant4/PTL \
            -DDATASETS_SOURCE_DIR=$MEMFS/sources/geant4/datasets \
            "-DDATASETS_LIST=G4EMLOW8.6.1;PhotonEvaporation6.1;G4NDL4.7.1;G4SAIDDATA2.0;G4ENSDFSTATE3.0;G4PARTICLEXS4.1"\
            -DLAZY_GENERATOR=$MEMFS/geant-web-application/utils/lazy_json_generator.py \
            -DEMSCRIPTEN_FILE_PACKAGER=$MEMFS/sources/emsdk-4.0.13/upstream/emscripten/tools/file_packager.py


        emmake make -C build -j 14

        mkdir -p $MEMFS/bucket-staging/geant4-wasm
        mkdir -p $MEMFS/bucket-staging/geant4-wasm/datafiles
        mkdir -p $MEMFS/bucket-staging/geant4-wasm/lazy_files_metadata
        

        yes | cp -f build/geant4_wasm.wasm $MEMFS/bucket-staging/geant4-wasm/geant4_wasm.wasm
        yes | cp -rf build/data/* $MEMFS/bucket-staging/geant4-wasm/datafiles
        yes | cp -rf build/js/*.metadata $MEMFS/bucket-staging/geant4-wasm/datafiles
        yes | cp -rf build/lazy_files/* $MEMFS/bucket-staging/geant4-wasm/lazy_files_metadata
    popd
popd