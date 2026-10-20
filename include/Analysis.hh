#ifndef ANALYSIS_HH
#define ANALYSIS_HH

#include "G4GenericAnalysisManager.hh"  // Correct header for the generic analysis manager

class Analysis {
public:
    static G4GenericAnalysisManager* Instance() {
        static G4GenericAnalysisManager* instance = G4GenericAnalysisManager::Instance("root");
        return instance;
    }
};

#endif

