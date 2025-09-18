#include "ActionInitialization.hh"

void ActionInitialization::Build() const {
    PrimaryGenerator *generator = new PrimaryGenerator;
    SetUserAction(generator);

    RunAction *runAction = new RunAction();
    SetUserAction(runAction);
}