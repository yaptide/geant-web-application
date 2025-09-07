#pragma once

#include "G4UserRunAction.hh"
#include "G4RootAnalysisManager.hh"

using G4AnalysisManager = G4RootAnalysisManager;

namespace gdml {
    class RunAction;
}

class gdml::RunAction : public G4UserRunAction {
public:
    void BeginOfRunAction(const G4Run *) override;
    void EndOfRunAction(const G4Run *) override;
};