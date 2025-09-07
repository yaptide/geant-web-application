#pragma once

#include "G4VUserActionInitialization.hh"
#include "GDML_PrimaryGenerator.hh"
#include "GDML_RunAction.hh"

namespace gdml {
    class ActionInitialization;
};

class gdml::ActionInitialization : public G4VUserActionInitialization {
public:
    void Build() const override;
};