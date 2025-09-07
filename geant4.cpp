#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

#include "geant4.hpp"

#include <iostream>

#include "G4UIExecutive.hh"
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4ScoringManager.hh"
#include "G4TScoreHistFiller.hh"
#include "G4RootAnalysisManager.hh"
#include "G4GDMLParser.hh"
#include "G4RootAnalysisManager.hh"

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "GDML_PhysicsList.hh"
#include "GDML_ActionInitialization.hh"
#include "GDML_DetectorConstruction.hh"

#include "QBBC.hh"

int Geant4_init() {
    auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

    runManager->SetUserInitialization(new B1::DetectorConstruction());
    auto physicsList = new QBBC();
    runManager->SetUserInitialization(physicsList);
    runManager->SetUserInitialization(new B1::ActionInitialization());
    
    runManager->Initialize();

    // get the pointer to the UI manager and set verbosities
    G4UImanager* UI = G4UImanager::GetUIpointer();
    UI->ApplyCommand("/run/verbose 1");
    UI->ApplyCommand("/event/verbose 1");
    UI->ApplyCommand("/tracking/verbose 1");

    // start a run
    int numberOfEvent = 3;
    runManager->BeamOn(numberOfEvent);

    // job termination
    // delete physicsList;
    delete runManager;

    return 0;
}

int Geant4_GDML() {
    G4RunManager *runManager = new G4RunManager;

    G4ScoringManager::GetScoringManager();

    // Physics list
    runManager->SetUserInitialization(new gdml::PhysicsList());

    // Detector construction
    G4GDMLParser parser;
    parser.SetOverlapCheck(true);

    parser.Read("geom.gdml");
    runManager->SetUserInitialization(new gdml::DetectorConstruction(parser.GetWorldVolume()));

    // Action initialization
    runManager->SetUserInitialization(new gdml::ActionInitialization());

    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetDefaultFileType("root");
    analysisManager->OpenFile("output");

    // instantiate template -- necessary to work
    new G4TScoreHistFiller<G4AnalysisManager>;

    G4UImanager *UImanager = G4UImanager::GetUIpointer();

    G4String command = "/control/execute ";
    G4String fileName = "init.mac";
    UImanager->ApplyCommand(command + fileName);

    return 0;
}

int Geant4_run() {
    std::cout << " Hello run \n";

    return -2;
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("Geant4_init", &Geant4_init);
    emscripten::function("Geant4_GDML", &Geant4_GDML);
    emscripten::function("Geant4_run", &Geant4_run);

}
#endif
