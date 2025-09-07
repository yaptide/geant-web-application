#include "GDML_PhysicsList.hh"

using namespace gdml;

PhysicsList::PhysicsList() {
    RegisterPhysics(new G4EmStandardPhysics());
}