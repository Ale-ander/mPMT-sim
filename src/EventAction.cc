#include "EventAction.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"
#include "G4ios.hh"
#include "G4AnalysisManager.hh"
#include "PMTHit.hh"
#include "G4SystemOfUnits.hh"
#include "RunAction.hh"
#include "G4RunManager.hh"

EventAction::EventAction()
    : G4UserEventAction(),
      fPMTHitCollectionID(-1) {}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event*) {
    // Initialization actions at the beginning of each event
}

void EventAction::EndOfEventAction(const G4Event* event) {
    G4HCofThisEvent* hce = event->GetHCofThisEvent();
    if (!hce) return;

    if (fPMTHitCollectionID == -1) {
        fPMTHitCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("PMTHitsCollection");
    }

    auto pmtHitsCollection = static_cast<G4THitsCollection<PMTHit>*>(hce->GetHC(fPMTHitCollectionID));

    if (!pmtHitsCollection) return;

    auto analysisManager = G4AnalysisManager::Instance();
    G4int eventID = event->GetEventID();

    std::map<int, int> pmtHitCounts;  // To count hits per PMT
    std::map<int, std::vector<G4double>> pmtHitTimes; //To save the PMT hit time per event
    int totalHits = 0; // Total hits in the event

    if (pmtHitsCollection) {
        G4int nofPMTHits = pmtHitsCollection->entries();
        G4cout << "Event " << eventID + 1 << " has " << nofPMTHits << " hits." << G4endl; // Debugging output

        for (G4int i = 0; i < nofPMTHits; i++) {
            PMTHit* hit = (*pmtHitsCollection)[i];
            int pmtNumber = hit->GetPMTNumber();
            G4double hitTime = hit->GetHitTime();

            if (pmtNumber >= 0 && pmtNumber < 19) {
                // Fill the ntuple corresponding to the PMT number
                analysisManager->FillNtupleIColumn(pmtNumber, 0, eventID + 1);
                analysisManager->FillNtupleIColumn(pmtNumber, 1, hit->GetTrackID());
                analysisManager->FillNtupleIColumn(pmtNumber, 2, hit->GetPMTNumber());
                analysisManager->FillNtupleDColumn(pmtNumber, 3, hit->GetHitPosition().x() / CLHEP::mm);
                analysisManager->FillNtupleDColumn(pmtNumber, 4, hit->GetHitPosition().y() / CLHEP::mm);
                analysisManager->FillNtupleDColumn(pmtNumber, 5, hit->GetHitPosition().z() / CLHEP::mm);
                analysisManager->FillNtupleDColumn(pmtNumber, 6, hit->GetHitTime() / CLHEP::ns);
                analysisManager->FillNtupleDColumn(pmtNumber, 7, hit->GetEnergy() / CLHEP::MeV);
                analysisManager->FillNtupleDColumn(pmtNumber, 8, hit->GetWavelength() / CLHEP::nm);
                analysisManager->FillNtupleDColumn(pmtNumber, 9, hit->GetLocalPosition().x() / CLHEP::mm);
                analysisManager->FillNtupleDColumn(pmtNumber, 10, hit->GetLocalPosition().y() / CLHEP::mm);
                analysisManager->FillNtupleDColumn(pmtNumber, 11, hit->GetLocalPosition().z() / CLHEP::mm);
                analysisManager->FillNtupleDColumn(pmtNumber, 12, hit->GetDirection().x());
                analysisManager->FillNtupleDColumn(pmtNumber, 13, hit->GetDirection().y());
                analysisManager->FillNtupleDColumn(pmtNumber, 14, hit->GetDirection().z());
                analysisManager->FillNtupleDColumn(pmtNumber, 15, hit->GetPixelPosition().x() / CLHEP::mm);
                analysisManager->FillNtupleDColumn(pmtNumber, 16, hit->GetPixelPosition().y() / CLHEP::mm);
                analysisManager->FillNtupleDColumn(pmtNumber, 17, hit->GetPixelPosition().z() / CLHEP::mm);
                analysisManager->FillNtupleIColumn(pmtNumber, 18, hit->GetDetectorID());
                analysisManager->AddNtupleRow(pmtNumber);

                // Count hits for CSV
                pmtHitCounts[pmtNumber]++;
                totalHits++; // Increment total hits count
                //To save PMT hit times
                pmtHitTimes[pmtNumber].push_back(hitTime);
            } else {
                G4cerr << "Invalid PMT number: " << pmtNumber << G4endl; // Debugging output
            }
        }

        // Write hit counts to CSV
        RunAction* runAction = const_cast<RunAction*>(static_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction()));
        for (const auto& pmtHits : pmtHitCounts) {
            int pmtID = pmtHits.first;
            int count = pmtHits.second;
            if (runAction->csvFile.is_open()) {
                runAction->csvFile << eventID + 1 << "," << pmtID << "," << count << ",\"[";
                
                auto& times = pmtHitTimes[pmtID];
                std::sort(times.begin(), times.end());
                for (size_t i = 0; i < times.size(); i++){
                    runAction->csvFile <<  times[i] / ns ;
                    if (i != times.size() - 1){
                        runAction->csvFile << ",";
                    }
                }
                runAction->csvFile << "]\"" << "\n";
                runAction->csvFile.flush(); // Ensure data is written to the file
                G4cout << "Writing to CSV file for PMT" << pmtID << ": " << eventID + 1 << "," << count << std::endl; // Debugging output
            } else {
                G4cerr << "Failed to write to CSV file for PMT" << pmtID << G4endl; // Debugging output
            }
        }

        // Write total hits to the new CSV file
        if (runAction->totalHitsFile.is_open()) {
            runAction->totalHitsFile << eventID << "," << totalHits << "\n";
            runAction->totalHitsFile.flush(); // Ensure data is written to the file
            G4cout << "Writing total hits to CSV file: " << eventID + 1 << "," << totalHits << G4endl; // Debugging output
        } else {
            G4cerr << "Failed to write to total hits CSV file" << G4endl; // Debugging output
        }
    }
}

