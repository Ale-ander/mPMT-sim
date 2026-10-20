#include "WCDSD.hh"
#include "WCDHit.hh"
#include "UserTrackInformation.hh"
#include "DetectorConstruction.hh"
#include "G4RunManager.hh"
#include "G4Material.hh"
#include "G4Step.hh"
#include "G4Transportation.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include <CLHEP/Units/PhysicalConstants.h>  // Include CLHEP constants

WCDSD::WCDSD(const G4String& name)
    : G4VSensitiveDetector(name), fHitsCollection(nullptr), fHCID(-1) {
    collectionName.insert("WCDHitsCollection");
    G4cout << "WCDSD: Created sensitive detector with name: " << name << G4endl;
}

WCDSD::~WCDSD() {}

void WCDSD::Initialize(G4HCofThisEvent* hce) {
    G4cout << "WCDSD: Initializing hits collection" << G4endl;
    fHitsCollection = new G4THitsCollection<WCDHit>(SensitiveDetectorName, collectionName[0]);
    if (fHCID < 0) {
        fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    }
    hce->AddHitsCollection(fHCID, fHitsCollection);
}

G4bool WCDSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/) {
    G4String particleName = aStep->GetTrack()->GetDefinition()->GetParticleName();

    if (aStep->GetPreStepPoint()->GetStepStatus() != fGeomBoundary) {
        return true;
    }

    G4bool status = true;

    const G4Track* track = aStep->GetTrack();
    auto* userTrackInfo = dynamic_cast<UserTrackInformation*>(track->GetUserInformation());

    if (!userTrackInfo) {
        track->SetUserInformation(new UserTrackInformation());
        userTrackInfo = dynamic_cast<UserTrackInformation*>(track->GetUserInformation());
    }

    G4int hitCollId = 0;
    G4int copyNb = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
    G4bool isScored = userTrackInfo->GetIsScored(SensitiveDetectorName + collectionName[hitCollId], copyNb);

    // Record the information concerning the primary particles injected in the WCD (i.e., at the boundary)
    if (aStep->GetPreStepPoint()->GetStepStatus() == fGeomBoundary && !isScored) {
        status = status && ProcessHit(aStep, hitCollId);
        userTrackInfo->SetIsScored(SensitiveDetectorName + collectionName[hitCollId], copyNb);
    }

    return status;
}

G4bool WCDSD::ProcessHit(const G4Step* aStep, const G4int& hitCollId) {
    const G4Track* track = aStep->GetTrack();
    const G4ParticleDefinition* particle = track->GetDefinition();

    static G4RunManager* runManager = G4RunManager::GetRunManager();
    static const auto* detector =
        dynamic_cast<const DetectorConstruction*>(runManager->GetUserDetectorConstruction());

   // static std::map<unsigned int, G4ThreeVector> listStations = detector->GetListStations();

    G4TouchableHistory* theTouchable = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());

    G4int pdg = particle->GetPDGEncoding();
    G4String particleName = particle->GetParticleName();
    G4double charge = particle->GetPDGCharge();

    G4StepPoint* stepPoint = aStep->GetPreStepPoint();
    G4VPhysicalVolume* physVolume = stepPoint->GetPhysicalVolume();

    G4double time = stepPoint->GetGlobalTime();
    G4double kineticEnergy = stepPoint->GetKineticEnergy();
    G4double wavelength =  CLHEP::h_Planck *  CLHEP::c_light / kineticEnergy;
    G4ThreeVector worldPos = stepPoint->GetPosition();

    G4ThreeVector hitPosition = theTouchable->GetHistory()->GetTopTransform().TransformPoint(worldPos);

    G4ThreeVector HitDirection = stepPoint->GetMomentumDirection();
    G4ThreeVector SDposition = physVolume->GetObjectTranslation();
    G4double weight = track->GetWeight();

    auto* hit = new WCDHit();

    hit->SetTrackID(track->GetTrackID());
    hit->SetPMTNumber(physVolume->GetCopyNo());
    hit->SetHitPosition(hitPosition);
    hit->SetHitTime(time);
    hit->SetEnergy(kineticEnergy);
    hit->SetWavelength(wavelength);
    hit->SetLocalPosition(worldPos);
    hit->SetDirection(HitDirection);
    hit->SetPixelPosition(SDposition);
    hit->SetDetectorID(physVolume->GetCopyNo());

    fHitsCollection->insert(hit); 
/*
    G4cout << "Hit detected in WCD: " << physVolume->GetCopyNo() 
           << ", Particle: " << particleName 
           << ", Energy: " << G4BestUnit(kineticEnergy, "Energy")
           << ", Time: " << G4BestUnit(time, "Time") 
           << ", Position: " << G4BestUnit(hitPosition, "Length") 
           << G4endl;
*/
    return true;
}

void WCDSD::EndOfEvent(G4HCofThisEvent* /*HCE*/) {
    std::map<G4int, G4int> pmtHitCount;

    G4int nofHits = fHitsCollection->entries();
    for (G4int i = 0; i < nofHits; i++) {
        G4int pmtNumber = (*fHitsCollection)[i]->GetPMTNumber();
        pmtHitCount[pmtNumber]++;
    }

    G4cout << G4endl << "----- WCD Hits Collection: " << nofHits << " hits -----" << G4endl;
    for (const auto& entry : pmtHitCount) {
        G4cout << "WCD " << entry.first << " : " << entry.second << " hits" << G4endl;
    } 
}

