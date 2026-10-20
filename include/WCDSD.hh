#ifndef WCDSD_HH
#define WCDSD_HH

#include "G4VSensitiveDetector.hh"
#include "G4THitsCollection.hh"
#include "WCDHit.hh"

class WCDSD : public G4VSensitiveDetector {
public:
    WCDSD(const G4String& name);
    virtual ~WCDSD();

    virtual void Initialize(G4HCofThisEvent* hce) override;
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) override;
    virtual void EndOfEvent(G4HCofThisEvent* hce) override;

private:
    G4THitsCollection<WCDHit>* fHitsCollection;
    G4int fHCID;

    G4bool ProcessHit(const G4Step* aStep, const G4int& hitCollId);
};

#endif

