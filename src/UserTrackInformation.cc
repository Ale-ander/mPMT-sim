#include "UserTrackInformation.hh"
#include <algorithm>

UserTrackInformation::UserTrackInformation() {
    fScoringSD.clear();
}

UserTrackInformation::~UserTrackInformation() {;}

void UserTrackInformation::SetIsScored(G4String name, G4int id) {
    if (!WasScored(name)) {
        std::vector<G4int> vid;
        fScoringSD[name] = vid;
    }

    if (!WasScored(name, id)) {
        fScoringSD[name].push_back(id);
    }
}

G4bool UserTrackInformation::GetIsScored(G4String name, G4int id) {
    return WasScored(name, id);
}

G4bool UserTrackInformation::WasScored(G4String name, G4int id) {
    if (WasScored(name)) {
        std::vector<G4int> vSD = fScoringSD[name];
        return (std::find(vSD.begin(), vSD.end(), id) != vSD.end());
    }
    return false;
}

G4bool UserTrackInformation::WasScored(G4String name) {
    return (fScoringSD.find(name) != fScoringSD.end());
}

