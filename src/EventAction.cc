#include "EventAction.hh"
#include "RunAction.hh"

extern void (*setCurrentProgress)(int);

EventAction::EventAction(RunAction* runAction) : fRunAction(runAction) {}

void EventAction::BeginOfEventAction(const G4Event* event)
{
  if (event->GetEventID() % 100 == 0 && setCurrentProgress)
    setCurrentProgress(event->GetEventID());
}


void EventAction::EndOfEventAction(const G4Event*)
{
}