#pragma once

#include <G4VUserDetectorConstruction.hh>

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
    DetectorConstruction(G4VPhysicalVolume *fWorld) : fWorld(fWorld) {};

    G4VPhysicalVolume *Construct() override { return fWorld; }
private:
    G4VPhysicalVolume *fWorld;
};