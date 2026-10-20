#include "G4UImessenger.hh"
#include "globals.hh"

class RunAction;
class G4UIcmdWithAString;

class RunMessenger : public G4UImessenger {
public:
    RunMessenger(RunAction* action);
    ~RunMessenger() override;

    void SetNewValue(G4UIcommand* command, G4String newValue) override;

private:
    RunAction* fAction;
    G4UIcmdWithAString* fSuffixCmd;
    G4UIcmdWithAString* dataFolderPath;
};
