#include <vector>
#include <G4RunManagerFactory.hh>
#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>
#include <G4String.hh>
#include <G4UImanager.hh>
#include "ActionInitialization.hh"
#include "PMTSD.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include <G4OpticalPhysics.hh>

/// - run any number of macros (put them as command-line arguments)
/// - start interactive UI mode with "-i" or "--interactive"

int main(int argc, char ** argv) {
    G4cout << "Application starting..." << G4endl;

    std::vector<G4String> macros;
    G4bool interactive = false;

    for (int i = 1; i < argc; i++) {
        G4String arg = argv[i];
        if (arg == "-i" || arg == "--interactive") {
            interactive = true;
        } else {
            macros.push_back(arg);
        }
    }

    G4UIExecutive* ui = nullptr;
    if (interactive) {
        ui = new G4UIExecutive(argc, argv);
    }

    auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial);
    runManager -> SetVerboseLevel(0);

    runManager -> SetUserInitialization(new DetectorConstruction());
    runManager -> SetUserInitialization(new PhysicsList());
    runManager -> SetUserInitialization(new ActionInitialization());

    G4VisManager* visManager = nullptr;
    if (ui) {
        visManager = new G4VisExecutive();
        visManager -> Initialize();
    }

    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    if (ui) {
        UImanager -> ApplyCommand("/control/execute ../macros/vis.mac");
    }

    for (const auto& macro : macros) {
        G4cout << "Executing macro: " << macro << G4endl;
        UImanager -> ApplyCommand("/control/execute " + macro);
    }

    if (ui) {
        ui -> SessionStart();
        delete ui;
    }

    delete visManager;
    delete runManager;

    G4cout << "Application successfully ended.\nBye :-)" << G4endl;
    return 0;
}