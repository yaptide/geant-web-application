#include "GDML_RunAction.hh"

using namespace gdml;

void RunAction::BeginOfRunAction(const G4Run *run) {
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
}

void RunAction::EndOfRunAction(const G4Run *run) {
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile(false);
}