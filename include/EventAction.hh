#pragma once

#include <G4UserEventAction.hh>
#include <G4Event.hh>
#include <globals.hh>

#include "RunAction.hh"

class EventAction : public G4UserEventAction
{
  public:
    EventAction(RunAction* runAction);
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

  private:
    RunAction* fRunAction = nullptr;
};