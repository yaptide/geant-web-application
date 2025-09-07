#include "GDML_ActionInitialization.hh"

using namespace gdml;

void ActionInitialization::Build() const {
    PrimaryGenerator *generator = new PrimaryGenerator;
    SetUserAction(generator);

    RunAction *runAction = new RunAction();
    SetUserAction(runAction);
}