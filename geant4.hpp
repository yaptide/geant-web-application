#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

#include <string>

int Geant4GDMRun(const std::string& gdml_file, const std::string& macro_file);

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_BINDINGS(geant4) {
    emscripten::function("Geant4GDMRun", &Geant4GDMRun);
}
#endif