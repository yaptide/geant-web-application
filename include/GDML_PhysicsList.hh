#pragma once

#include "G4VModularPhysicsList.hh"
#include "G4EmStandardPhysics.hh"

namespace gdml {
    class PhysicsList;
}

class gdml::PhysicsList : public G4VModularPhysicsList {
public:
    PhysicsList();
};