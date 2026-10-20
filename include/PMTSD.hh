#ifndef PMTSD_HH
#define PMTSD_HH

#include "G4VSensitiveDetector.hh"
#include "G4THitsCollection.hh"
#include "PMTHit.hh"
#include <map>

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class PMTSD : public G4VSensitiveDetector {
public:
    PMTSD(const G4String& name);
    virtual ~PMTSD();

    virtual void Initialize(G4HCofThisEvent* HCE) override;
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) override;
    virtual void EndOfEvent(G4HCofThisEvent* HCE) override;
    virtual void clear();
    virtual void DrawAll();
    virtual void PrintAll();

private:
    G4THitsCollection<PMTHit>* fHitsCollection;
    G4bool ProcessHit(G4Step* aStep, G4TouchableHistory* theTouchable, G4int id);
};

#endif

