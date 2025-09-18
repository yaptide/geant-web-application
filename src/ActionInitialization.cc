#include "ActionInitialization.hh"
#include "EventAction.hh"

void ActionInitialization::Build() const {
    PrimaryGenerator *generator = new PrimaryGenerator;
    SetUserAction(generator);

    RunAction *runAction = new RunAction();
    SetUserAction(runAction);

    EventAction *eventAction = new EventAction(runAction);
    SetUserAction(eventAction);
}