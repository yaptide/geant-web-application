#pragma once

#include "G4VUserDetectorConstruction.hh"

namespace gdml
{
    class DetectorConstruction;
};


class gdml::DetectorConstruction : public G4VUserDetectorConstruction {
public:
    DetectorConstruction(G4VPhysicalVolume *fWorld) : fWorld(fWorld) {};

    G4VPhysicalVolume *Construct() override { return fWorld; }
private:
    G4VPhysicalVolume *fWorld;
};