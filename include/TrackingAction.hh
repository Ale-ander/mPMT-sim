#ifndef TRACKINGACTION_HH
#define TRACKINGACTION_HH

#include "G4UserTrackingAction.hh"
#include "G4Timer.hh"

class TrackingAction : public G4UserTrackingAction {
public:
    TrackingAction();
    virtual ~TrackingAction();

    virtual void PreUserTrackingAction(const G4Track* aTrack) override;
    virtual void PostUserTrackingAction(const G4Track* aTrack) override;

private:
    G4int fNTotalSimulated;
    G4double fTimeTotal;
    G4Timer fTimer;
};

#endif

