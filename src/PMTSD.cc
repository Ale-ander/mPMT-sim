#include "PMTSD.hh"
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

#include <sstream>

using namespace CLHEP;

PMTSD::PMTSD(const G4String& name):G4VSensitiveDetector(name),fHitsCollection(nullptr) 
{
  collectionName.insert("PMTHitsCollection");
}


PMTSD::~PMTSD()
{;}


void PMTSD::Initialize(G4HCofThisEvent* HCE)
{
  static int HCID1 = -1;
  fHitsCollection =
    new G4THitsCollection<PMTHit>(SensitiveDetectorName, collectionName[0]);

  if (HCID1 < 0) {
    HCID1 = GetCollectionID(0);
}

  HCE->AddHitsCollection(HCID1,fHitsCollection);
}


G4bool PMTSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*unused*/)
{
  G4String particleName = aStep->GetTrack()->GetDefinition()->GetParticleName();

  G4bool status = false;

  auto * theTouchable
    = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());

  auto * posStepPoint = aStep->GetPostStepPoint();

  G4OpBoundaryProcessStatus boundaryStatus = Undefined;
  static G4OpBoundaryProcess* nboundary = nullptr;

  if (particleName == "opticalphoton"){
    if (!nboundary){ // Enter here just the first time
      // Get process manager for the optical photon category
      // Loop through the associated processes to get the pointer to the OpBoundary one
      auto * pm = aStep->GetTrack()->GetDefinition()->GetProcessManager();
      G4cout << "boundary check" << G4endl; 
      G4int nprocesses = pm->GetProcessListLength();
      G4ProcessVector* pv = pm->GetProcessList();
      G4cout << "Processes associated to opticalphoton : " << G4endl;
      for(G4int i=0;i<nprocesses;i++){
        G4cout << (*pv)[i]->GetProcessName() << G4endl;
        if ((*pv)[i]->GetProcessName() == "OpBoundary"){
          nboundary = (G4OpBoundaryProcess*)(*pv)[i];
          // break;
        }
      }
      }

    //Check to see if the particle was actually at a boundary

    if (posStepPoint->GetStepStatus() == fGeomBoundary) {
      boundaryStatus=nboundary->GetStatus();
      //G4cout <<"AAA " << nboundary->GetProcessName() << "  " << boundaryStatus << G4endl;
      switch(boundaryStatus){
        case Absorption:
        break;
        case Detection:
        status = ProcessHit(aStep,theTouchable,0);
        break;
	    //case 13:

	  //status = ProcessHit(aStep,theTouchable,0);
	  //break; 
        default:
        break;
      }
    }
  } // end if opticalphoton
  else if (aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) {
    status = ProcessHit(aStep,theTouchable,aStep->GetTrack()->GetDefinition()->GetPDGEncoding());
  } else {
    status = true;
}

  return status;
}


G4bool PMTSD::ProcessHit(G4Step* aStep, G4TouchableHistory* theTouchable, G4int id)
{
  static G4RunManager * runManager = G4RunManager::GetRunManager();
  static const auto * detector =
  dynamic_cast<const DetectorConstruction*>(runManager->GetUserDetectorConstruction());
  //  static std::map<unsigned int,G4ThreeVector> listStations = detector->GetListStations();

  const G4Event * event = runManager->GetCurrentEvent();
  G4int primaryId = event->GetPrimaryVertex()->GetPrimary()->GetPDGcode();
  G4Track* track = aStep->GetTrack();
  G4String particleName = track->GetDefinition()->GetParticleName();
  G4double charge = track->GetDefinition()->GetPDGCharge();

    //  G4String volumeName = fPhysVolume->GetName();
  G4StepPoint* stepPoint = 0;
  if (particleName == "opticalphoton") {// Optical photons are treated differently from other particles
    stepPoint = aStep->GetPostStepPoint();
  }
  else {
    stepPoint = aStep->GetPreStepPoint();
  }

  G4VPhysicalVolume* physVolume = stepPoint->GetPhysicalVolume();
  G4double fGlobalTOF = track->GetGlobalTime();
  G4double kineticEnergy = track->GetKineticEnergy();
  G4double Wavelength = h_Planck*c_light/kineticEnergy;
  G4ThreeVector worldPos = stepPoint->GetPosition();
  G4ThreeVector HitPosition =
    theTouchable->GetHistory()->GetTopTransform().TransformPoint(worldPos);
  G4ThreeVector HitDirection = stepPoint->GetMomentumDirection();
  G4double weight = track->GetWeight();

  auto * hit = new class PMTHit();

  // Search for the WCD and PMT volumes

  G4int wcdId = 0, pmtId = 0;
  G4String name;
  G4ThreeVector pmtPos;

  // In order to correctly get the PMT id and position we have to check both
  // the actual physical volume and the volumes hierarchy. Note that, e.g., the physical volume of the photocathode inside the glass dome has position (0,0,0).
  if ((physVolume->GetName()).substr(0,3) == "PMT") {
    pmtId = physVolume->GetCopyNo();
    pmtPos = physVolume->GetObjectTranslation();
  }

  for (int i=0, n=theTouchable->GetHistoryDepth(); i<n; i++) {
    name = theTouchable->GetVolume(i)->GetName();
    //G4cout << name << " BBB " << theTouchable->GetVolume(i)->GetCopyNo() << G4endl;
    if (name.substr(0,3) == "PMT") {
      pmtId = theTouchable->GetVolume(i)->GetCopyNo();
      pmtPos = theTouchable->GetVolume(i)->GetObjectTranslation();
    }

    if (name.substr(0,3) == "WCD") {
      wcdId = theTouchable->GetVolume(i)->GetCopyNo();
    }
  }


   hit->SetTrackID(track->GetTrackID());
    hit->SetPMTNumber(pmtId);
    hit->SetHitPosition(HitPosition);
    hit->SetHitTime(fGlobalTOF);
    hit->SetEnergy(kineticEnergy);
    hit->SetWavelength(Wavelength);
    hit->SetLocalPosition(worldPos);
    hit->SetDirection(HitDirection);
    hit->SetPixelPosition(pmtPos);
    hit->SetDetectorID(pmtId);
  
    /*
  hit->SetEnergy(kineticEnergy);
  hit->SetWavelength(Wavelength);
  hit->SetTime(fGlobalTOF);
  hit->SetWeight(weight);
  hit->SetCharge(charge);
  hit->SetPosition(HitPosition);
  hit->SetWorldPosition(worldPos);
  hit->SetDirection(HitDirection);
  hit->SetPixelPosition(pmtPos);
  hit->SetPDG(id);
  hit->SetParentID(track->GetParentID());
  hit->SetPrimaryID(primaryId);
  hit->SetDetectorID(pmtId);
  hit->SetStationID(wcdId);
  // hit->SetStationPosition(listStations[hit->GetStationID()]);
  hit->SetDetectorPosition(pmtPos);
    */
  fHitsCollection->insert(hit);

  // G4cout << pmtId << " --  " <<  particleName << G4endl;
  // G4cout << hit->GetStationPosition()/m << "   " << hit->GetDetectorPosition()/m << G4endl;

  return true;
}


void PMTSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{
  /*static G4int HCID = -1;
  if (HCID < 0)
    HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);

  HCE->AddHitsCollection(HCID,fHitsCollection);*/




    std::map<G4int, G4int> pmtHitCount;

    G4int nofHits = fHitsCollection->entries();
    for (G4int i = 0; i < nofHits; i++) {
        G4int pmtNumber = (*fHitsCollection)[i]->GetPMTNumber();
        pmtHitCount[pmtNumber]++;
    }

    G4cout << G4endl << "----- PMTSD Hits Collection: " << nofHits << " hits -----" << G4endl;
    for (const auto& entry : pmtHitCount) {
        G4cout << "PMT" << entry.first << " : " << entry.second << " hits" << G4endl;
    }
    G4cout << "PMTSD: End of event" << G4endl;



}


void PMTSD::clear()
{
}


void PMTSD::DrawAll()
{
}


void PMTSD::PrintAll()
{
}
