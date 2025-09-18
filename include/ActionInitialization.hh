#pragma once

#include "G4VUserActionInitialization.hh"
#include "PrimaryGenerator.hh"
#include "RunAction.hh"

class ActionInitialization : public G4VUserActionInitialization {
public:
    void Build() const override;
};