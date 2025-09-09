#!/bin/bash
set -e # exit script on any command fail

module add cmake

EMSDK_VERSION=4.0.13
WGET_FLAG=-N

npm config set prefix ~/.npm_global

FORCE=0

unpack_file() {
    FILE=$1
    TARGET_DIR=$2 

    if [[ $FORCE -eq 1 || ! -d $TARGET_DIR ]]; then
        tar xvfz ${FILE}
    fi
} 

pushd $MEMFS

    mkdir -p sources/geant4/datasets
    pushd sources
        mkdir -p downloads
        pushd downloads
            wget ${WGET_FLAG} https://cern.ch/geant4-data/datasets/G4NDL.4.7.1.tar.gz
            wget ${WGET_FLAG} https://cern.ch/geant4-data/datasets/G4EMLOW.8.6.1.tar.gz
            wget ${WGET_FLAG} https://cern.ch/geant4-data/datasets/G4PhotonEvaporation.6.1.tar.gz
            wget ${WGET_FLAG} https://cern.ch/geant4-data/datasets/G4RadioactiveDecay.6.1.2.tar.gz
            wget ${WGET_FLAG} https://cern.ch/geant4-data/datasets/G4PARTICLEXS.4.1.tar.gz
            wget ${WGET_FLAG} https://cern.ch/geant4-data/datasets/G4PII.1.3.tar.gz
            wget ${WGET_FLAG} https://cern.ch/geant4-data/datasets/G4RealSurface.2.2.tar.gz
            wget ${WGET_FLAG} https://cern.ch/geant4-data/datasets/G4SAIDDATA.2.0.tar.gz
            wget ${WGET_FLAG} https://cern.ch/geant4-data/datasets/G4ABLA.3.3.tar.gz
            wget ${WGET_FLAG} https://cern.ch/geant4-data/datasets/G4INCL.1.2.tar.gz
            wget ${WGET_FLAG} https://cern.ch/geant4-data/datasets/G4ENSDFSTATE.3.0.tar.gz
            wget ${WGET_FLAG} https://cern.ch/geant4-data/datasets/G4CHANNELING.1.0.tar.gz
            wget ${WGET_FLAG} https://cern.ch/geant4-data/datasets/G4TENDL.1.4.tar.gz
            wget ${WGET_FLAG} https://cern.ch/geant4-data/datasets/G4NUDEXLIB.1.0.tar.gz
            wget ${WGET_FLAG} https://cern.ch/geant4-data/datasets/G4URRPT.1.1.tar.gz

            wget ${WGET_FLAG} https://github.com/emscripten-core/emsdk/archive/refs/tags/${EMSDK_VERSION}.tar.gz
            wget ${WGET_FLAG} https://github.com/libexpat/libexpat/releases/download/R_2_6_4/expat-2.6.4.tar.gz
            wget ${WGET_FLAG} https://dlcdn.apache.org/xerces/c/3/sources/xerces-c-3.3.0.tar.gz
            wget ${WGET_FLAG} https://gitlab.cern.ch/geant4/geant4/-/archive/v11.3.2/geant4-v11.3.2.tar.gz
        popd

        pushd geant4/datasets
            unpack_file ../../downloads/G4NDL.4.7.1.tar.gz G4NDL4.7.1
            unpack_file ../../downloads/G4EMLOW.8.6.1.tar.gz G4EMLOW8.6.1
            unpack_file ../../downloads/G4PhotonEvaporation.6.1.tar.gz PhotonEvaporation6.1
            unpack_file ../../downloads/G4RadioactiveDecay.6.1.2.tar.gz RadioactiveDecay6.1.2
            unpack_file ../../downloads/G4PARTICLEXS.4.1.tar.gz G4PARTICLEXS4.1
            unpack_file ../../downloads/G4PII.1.3.tar.gz G4PII1.3
            unpack_file ../../downloads/G4RealSurface.2.2.tar.gz RealSurface2.2
            unpack_file ../../downloads/G4SAIDDATA.2.0.tar.gz G4SAIDDATA2.0
            unpack_file ../../downloads/G4ABLA.3.3.tar.gz G4ABLA3.3
            unpack_file ../../downloads/G4INCL.1.2.tar.gz G4INCL1.2
            unpack_file ../../downloads/G4ENSDFSTATE.3.0.tar.gz G4ENSDFSTATE3.0
            unpack_file ../../downloads/G4CHANNELING.1.0.tar.gz G4CHANNELING1.0
            unpack_file ../../downloads/G4TENDL.1.4.tar.gz G4TENDL1.4
            unpack_file ../../downloads/G4NUDEXLIB.1.0.tar.gz G4NUDEXLIB1.0
            unpack_file ../../downloads/G4URRPT.1.1.tar.gz G4URRPT1.1
        popd

        unpack_file downloads/${EMSDK_VERSION}.tar.gz emsdk-${EMSDK_VERSION}
        unpack_file downloads/expat-2.6.4.tar.gz expat-2.6.4
        unpack_file downloads/xerces-c-3.3.0.tar.gz xerces-c-3.3.0
        unpack_file downloads/geant4-v11.3.2.tar.gz geant4-v11.3.2

        pushd emsdk-${EMSDK_VERSION}
            ./emsdk install ${EMSDK_VERSION}
            npm install -g typescript
            ./emsdk activate ${EMSDK_VERSION}
            source emsdk_env.sh
        popd

        pushd xerces-c-3.3.0
            mkdir -p {build,install}
            pushd build
                emcmake cmake                           \
                    -Dnetwork-accessor=socket           \
                    -DCMAKE_BUILD_TYPE=Release          \
                    -DCMAKE_INSTALL_PREFIX=../install   \
                    ..

                emmake make -j
                make install
            popd
        popd

        pushd expat-2.6.4
            mkdir -p {build,install}
            pushd build
                emcmake cmake -DCMAKE_INSTALL_PREFIX=../install ..
                emmake make -j
                make install
            popd
        popd

        pushd geant4 
            mkdir -p {build,install}
            
            pushd build
                emcmake cmake \
                    -DCMAKE_INSTALL_PREFIX=../install \
                    -DGEANT4_USE_SYSTEM_EXPAT=ON \
                    -DXercesC_INCLUDE_DIR=../../xerces-c-3.3.0/install/include \
                    -DXercesC_LIBRARY=../../xerces-c-3.3.0/install \
                    -DEXPAT_INCLUDE_DIR=../../expat-2.6.4/install/include \
                    -DEXPAT_LIBRARY=../../expat-2.6.4/install/ \
                    -DBUILD_STATIC_LIBS=ON \
                    -DGEANT4_BUILD_STORE_TRAJECTORY=OFF \
                    -DBUILD_SHARED_LIBS=OFF \
                    -DGEANT4_BUILD_MULTITHREADED=OFF \
                    -DGEANT4_BUILD_BUILTIN_BACKTRACE=OFF \
                    -DGEANT4_INSTALL_DATA=OFF \
                    -DGEANT4_USE_GDML=ON \
                    ../../geant4-v11.3.2
            
                emmake make -j 14
                make install
            popd
        popd

    popd

    mkdir -p bucket-staging/geant4-wasm/datasets

    cp -au sources/geant4/datasets/. bucket-staging/geant4-wasm/datasets/.
popd
