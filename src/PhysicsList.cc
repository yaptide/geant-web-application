#include "PhysicsList.hh"

PhysicsList::PhysicsList() {
    RegisterPhysics(new G4EmStandardPhysics());
}