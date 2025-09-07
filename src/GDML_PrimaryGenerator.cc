#include "GDML_PrimaryGenerator.hh"

using namespace gdml;

PrimaryGenerator::PrimaryGenerator() {
    fParticleSource = new G4GeneralParticleSource();
}

PrimaryGenerator::~PrimaryGenerator() {
    delete fParticleSource;
}

void PrimaryGenerator::GeneratePrimaries(G4Event *anEvent) {
    fParticleSource->GeneratePrimaryVertex(anEvent);
}