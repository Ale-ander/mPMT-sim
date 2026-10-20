#include "RunMessenger.hh"
#include "RunAction.hh"
#include "G4UIcmdWithAString.hh"

RunMessenger::RunMessenger(RunAction* action) : fAction(action) {
    fSuffixCmd = new G4UIcmdWithAString("/output/setSuffix", this);
    fSuffixCmd->SetGuidance("Set suffix for output files");
    fSuffixCmd->SetParameterName("suffix", false);

    dataFolderPath = new G4UIcmdWithAString("/output/dataPath", this);
    dataFolderPath->SetGuidance("Set the base folder path for output files");
    dataFolderPath->SetParameterName("dataFolder", false);
}

RunMessenger::~RunMessenger() {
    delete fSuffixCmd;
    delete dataFolderPath;
}

void RunMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if (command == fSuffixCmd) {
        fAction->SetSuffix(newValue);
    }
    if (command == dataFolderPath){
        fAction->SetDataFolderPath(newValue);
    }
}
