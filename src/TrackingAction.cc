#include "TrackingAction.hh"
#include "UserTrackInformation.hh"
#include "DetectorConstruction.hh"
#include "PMTSpherical.hh"
#include "G4RunManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

TrackingAction::TrackingAction() : G4UserTrackingAction(), fNTotalSimulated(0), fTimeTotal(0) {
    fTimer.Start();
}

TrackingAction::~TrackingAction() {}

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
    static G4int nPrimaries = 0;

    G4String particleName = aTrack->GetDefinition()->GetParticleName();

    // Apply quantum efficiency before tracking. Optical photons only!
 /*   if (particleName == "opticalphoton") {

     G4cout<< " i saw optical photon"<<G4endl;
        const auto* runManager = G4RunManager::GetRunManager();
        const auto* detector = dynamic_cast<const DetectorConstruction*>(runManager->GetUserDetectorConstruction());

        // Get the quantum efficiency for the current photon's energy
        G4double efficiency = detector->GetPhotoDetectors()[0]->GetQuantumEfficiency(aTrack->GetKineticEnergy());

        if (G4UniformRand() > efficiency) {
            G4cout << "TrackingAction: Killing optical photon due to quantum efficiency" << G4endl;
            fpTrackingManager->GetTrack()->SetTrackStatus(fStopAndKill);
        }
    } else if (!aTrack->GetUserInformation()) {
        aTrack->SetUserInformation(new UserTrackInformation());

        if (aTrack->GetParentID() == 0) {
            nPrimaries++;
        }

        if (nPrimaries >= 100) {
            fTimer.Stop();
            G4double usertime = fTimer.GetUserElapsed();

            fNTotalSimulated += nPrimaries;
            fTimeTotal += usertime;

            G4cout << "# simulated particles/time/particles per second (partial/total): "
                   << nPrimaries << "  " << usertime << "  " << float(nPrimaries) / usertime << "  /  "
                   << fNTotalSimulated << "  " << fTimeTotal << "  " << float(fNTotalSimulated) / fTimeTotal << G4endl;

            // Reset counters
            nPrimaries = 0;
            fTimer.Start();
        }
    } */
}

void TrackingAction::PostUserTrackingAction(const G4Track* /*unused*/) {
    // Currently, nothing is done in this method
}

