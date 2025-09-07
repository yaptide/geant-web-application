cmake_minimum_required(VERSION 3.22)

project(geant4_wasm)

set (CMAKE_CXX_STANDARD 17)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

if (DEFINED EMSCRIPTEN)
        set(EXPAT_INCLUDE_DIR "/workspaces/cpp/sources/expat-2.6.4/wasm/install/include")
        set(EXPAT_LIBRARY "/workspaces/cpp/sources/expat-2.6.4/wasm/install/lib/libexpat.a")
        set(XercesC_INCLUDE_DIR "/workspaces/cpp/sources/xerces-c-3.3.0/wasm/install/include")
        set(XercesC_LIBRARY "/workspaces/cpp/sources/xerces-c-3.3.0/wasm/install/lib/libxerces-c-3.3.a")
        set(Geant4_DIR "/workspaces/cpp/geant4/wasm/install/lib/cmake/Geant4")
        set(PTL_DIR "/workspaces/cpp/geant4/wasm/install/lib/cmake/Geant4/PTL")

        find_package(Geant4 REQUIRED ui_all)

        add_executable(geant4_wasm geant4.cpp geant4.hpp ${sources} ${headers})

        target_include_directories(geant4_wasm PRIVATE include)
        target_link_libraries(geant4_wasm PRIVATE ${Geant4_LIBRARIES})

        set_target_properties(geant4_wasm PROPERTIES LINK_FLAGS 
                        "-s WASM=1 \
                         -s MODULARIZE=1 \
                         -s EXPORT_ES6=1 \
                         -s EXPORT_NAME=createWasmModule \
                         -s TOTAL_MEMORY=512MB \
                         -s ENVIRONMENT='worker' \
                         -s FORCE_FILESYSTEM  \
                         -s EXPORTED_RUNTIME_METHODS='[\"FS\"]' \
                         --use-preload-cache \
                         --pre-js ./setup_env.js \
                         -lembind")

        target_link_options(geant4_wasm PRIVATE
                --emit-tsd "geant4_wasm.d.ts"
        )

        set(DATASETS_SOURCE_DIR "/workspaces/cpp/geant4/datasets")
        set(DATASETS_LIST 
                "G4EMLOW8.6.1"
                "PhotonEvaporation6.1"
                "G4NDL4.7.1"
                "G4SAIDDATA2.0"
                "G4ENSDFSTATE3.0"
                "G4PARTICLEXS4.1"
        )
        set(DATASETS_TARGET_DIR "data")
        set(JS_TARGET_DIR "js")

        set(EMSCRIPTEN_FILE_PACKAGER "$ENV{EMSDK}/upstream/emscripten/tools/file_packager.py")
        find_package(Python REQUIRED)
        foreach(dataset ${DATASETS_LIST})
                set(DATASET_SOURCE_DIR "${DATASETS_SOURCE_DIR}/${dataset}")
                set(DATASET_MAP_DIR "${DATASETS_TARGET_DIR}/${dataset}")
                set(DATASET_TARGET "${DATASET_MAP_DIR}.data")
                set(JS_TARGET "${JS_TARGET_DIR}/preload_${dataset}.js")
                set(DATASET_FILE "${DATASET_SOURCE_DIR}@${DATASET_MAP_DIR}")
                add_custom_command(
                        TARGET geant4_wasm POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/${DATASETS_TARGET_DIR}
                        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/${JS_TARGET_DIR}
                        COMMAND ${Python_EXECUTABLE} ${EMSCRIPTEN_FILE_PACKAGER} ${PROJECT_BINARY_DIR}/${DATASET_TARGET}
                                                        --preload ${DATASET_FILE}
                                                        --js-output="${PROJECT_BINARY_DIR}/${JS_TARGET}" 
                                                        --quiet 
                                                        --use-preload-cache
                                                        --export-es6
                                                        --no-node
                                                        --separate-metadata
                        COMMENT "Packaging dataset ${dataset} for Emscripten"
                )
        endforeach(dataset ${DATASETS_LIST})

        set(LAZY_GENERATOR "/workspaces/geant-wasm/emscripten_utils/lazy_json_generator.py")
        find_package(Python REQUIRED)
        foreach(dataset ${DATASETS_LIST})
                set(DATASET_SOURCE_DIR "${DATASETS_SOURCE_DIR}/${dataset}")
                add_custom_command(
                        TARGET geant4_wasm POST_BUILD
                        COMMAND ${Python_EXECUTABLE} ${LAZY_GENERATOR} 
                                --input "${DATASET_SOURCE_DIR}"
                                --url "/geant4_wasm/data/${dataset}"
                                --output "${PROJECT_BINARY_DIR}/lazy_files/load_${dataset}.json"
                                --vpath "/data/${dataset}"
                        COMMENT "Packaging dataset ${dataset} for Emscripten"
                )
        endforeach(dataset ${DATASETS_LIST})
else()
        set(EXPAT_INCLUDE_DIR "/workspaces/cpp/sources/expat-2.6.4/install/include")
        set(EXPAT_LIBRARY "/workspaces/cpp/sources/expat-2.6.4/install/lib/libexpat.so")
        set(XercesC_INCLUDE_DIR "/workspaces/cpp/sources/xerces-c-3.3.0/install/include")
        set(XercesC_LIBRARY "/workspaces/cpp/sources/xerces-c-3.3.0/install/lib/libxerces-c.so")

        find_package(Geant4 PATHS "/workspaces/cpp/geant4/install" REQUIRED ui_all)

	add_library(geant4_wasm geant4.cpp ${sources} ${headers})

        target_include_directories(geant4_wasm PRIVATE include)
        target_link_libraries(geant4_wasm PRIVATE ${Geant4_LIBRARIES})
endif()

file(GLOB SCRIPTS ${PROJECT_SOURCE_DIR}/scripts/*)

foreach(_script ${SCRIPTS})
        get_filename_component(_script ${_script} NAME)
        configure_file(
        ${PROJECT_SOURCE_DIR}/scripts/${_script}
        ${PROJECT_BINARY_DIR}/${_script}
        COPYONLY
        )
endforeach()