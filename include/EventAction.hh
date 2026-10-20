#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include <G4UserEventAction.hh>
#include <G4Event.hh>
#include <G4THitsCollection.hh>
#include "PMTHit.hh"
#include "WCDHit.hh"

class EventAction : public G4UserEventAction {
public:
    EventAction();
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

private:
    G4int fPMTHitCollectionID;
    G4int fWCDHitCollectionID;
};

#endif

