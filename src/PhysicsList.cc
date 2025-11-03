#include "PhysicsList.hh"
// #include "G4EmStandardPhysics_option4.hh"
// #include "G4HadronElasticPhysicsHP.hh"
// #include "G4HadronPhysicsQGSP_BIC_HP.hh"
// #include "G4IonPhysics.hh"
// #include "G4DecayPhysics.hh"
// #include "G4RadioactiveDecayPhysics.hh"
// #include "G4StoppingPhysics.hh"
// #include "QBBC.hh"

// PhysicsList::PhysicsList()  {
//     // Electromagnetic physics (most accurate for medical applications)
//     RegisterPhysics(new G4EmStandardPhysics_option4());
    
//     // Hadron elastic scattering with high-precision neutrons
//     RegisterPhysics(new G4HadronElasticPhysicsHP());
    
//     // Hadron inelastic physics (protons, neutrons, pions)
//     RegisterPhysics(new G4HadronPhysicsQGSP_BIC_HP());
    
//     // // Ion interactions (for carbon, oxygen, heavy ions)
//     RegisterPhysics(new G4IonPhysics());
    
//     // Particle decay
//     RegisterPhysics(new G4DecayPhysics());
    
//     // Radioactive decay (optional, for activation studies)
//     RegisterPhysics(new G4RadioactiveDecayPhysics());
    
//     // Stopping physics (muon capture, antiproton annihilation)
//     RegisterPhysics(new G4StoppingPhysics());
//  }