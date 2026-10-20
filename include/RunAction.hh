// RunAction.hh
#include "G4UserRunAction.hh"
#include <fstream>
#include "G4AnalysisManager.hh"

class RunMessenger;

class RunAction : public G4UserRunAction {
public:
    RunAction();
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void EndOfRunAction(const G4Run*) override;

    void SetSuffix(const G4String& s) { fOutputSuffix = s;}
    void SetDataFolderPath(const G4String& s) {outputFolder = s;}

    std::ofstream csvFile;
    std::ofstream totalHitsFile; // New member for total hits CSV file

    std::string rootFolder;
    std::string csvFolder;
    std::string totalFolder;

private:
    std::string outputFolder; // New variable for directories
    G4AnalysisManager* analysisManager; // Declare the analysis manager

    G4String fOutputSuffix;
    RunMessenger* fMessenger;

};

