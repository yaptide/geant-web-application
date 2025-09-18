#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

#include <iostream>

#include <G4UIExecutive.hh>
#include <G4RunManagerFactory.hh>
#include <G4UImanager.hh>
#include <G4ScoringManager.hh>
#include <G4TScoreHistFiller.hh>
#include <G4RootAnalysisManager.hh>
#include <G4GDMLParser.hh>
#include <G4RootAnalysisManager.hh>

#include <QBBC.hh>

#include "PhysicsList.hh"
#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"

#include "geant4.hpp"

void (*setCurrentProgress)(int);

void Geant4SetProgressFunction(uint64_t functionPointer) {
    setCurrentProgress = reinterpret_cast<void (*)(int)>(functionPointer);
}

int Geant4GDMRun(const std::string& gdml_file, const std::string& macro_file) {
    G4RunManager *runManager = new G4RunManager;

    G4ScoringManager::GetScoringManager();

    // Physics list
    runManager->SetUserInitialization(new PhysicsList());

    // Detector construction
    G4GDMLParser parser;
    parser.SetOverlapCheck(true);

    parser.Read(gdml_file);
    runManager->SetUserInitialization(new DetectorConstruction(parser.GetWorldVolume()));

    // Action initialization
    runManager->SetUserInitialization(new ActionInitialization());

    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetDefaultFileType("root");
    analysisManager->OpenFile("output");

    // instantiate template -- necessary to work
    new G4TScoreHistFiller<G4AnalysisManager>;

    G4UImanager *UImanager = G4UImanager::GetUIpointer();

    G4String command = "/control/execute ";
    G4String fileName = macro_file;
    UImanager->ApplyCommand(command + fileName);

    return 0;
}
