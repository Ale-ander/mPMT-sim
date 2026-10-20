#ifndef USER_TRACK_INFORMATION_HH
#define USER_TRACK_INFORMATION_HH

#include "G4VUserTrackInformation.hh"
#include "G4String.hh"
#include <map>
#include <vector>

class UserTrackInformation : public G4VUserTrackInformation {
public:
    UserTrackInformation();
    virtual ~UserTrackInformation();

    void SetIsScored(G4String name, G4int id);
    G4bool GetIsScored(G4String name, G4int id);

private:
    G4bool WasScored(G4String name, G4int id);
    G4bool WasScored(G4String name);

    std::map<G4String, std::vector<G4int>> fScoringSD;
};

#endif

