#include "WCDHit.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4Circle.hh"
#include "G4VisAttributes.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

G4Allocator<WCDHit> WCDHitAllocator;

WCDHit::WCDHit()
    : fTrackID(-1), fPMTNumber(-1), fHitPosition(G4ThreeVector()), fHitTime(0.),
      fEnergy(0.), fWavelength(0.), fLocalPosition(G4ThreeVector()),
      fDirection(G4ThreeVector()), fPixelPosition(G4ThreeVector()), fDetectorID(-1) {}

WCDHit::~WCDHit() {}

WCDHit::WCDHit(const WCDHit& right)
    : G4VHit(), fTrackID(right.fTrackID), fPMTNumber(right.fPMTNumber),
      fHitPosition(right.fHitPosition), fHitTime(right.fHitTime), fEnergy(right.fEnergy),
      fWavelength(right.fWavelength), fLocalPosition(right.fLocalPosition),
      fDirection(right.fDirection), fPixelPosition(right.fPixelPosition),
      fDetectorID(right.fDetectorID) {}

const WCDHit& WCDHit::operator=(const WCDHit& right) {
    if (this != &right) {
        fTrackID = right.fTrackID;
        fPMTNumber = right.fPMTNumber;
        fHitPosition = right.fHitPosition;
        fHitTime = right.fHitTime;
        fEnergy = right.fEnergy;
        fWavelength = right.fWavelength;
        fLocalPosition = right.fLocalPosition;
        fDirection = right.fDirection;
        fPixelPosition = right.fPixelPosition;
        fDetectorID = right.fDetectorID;
    }
    return *this;
}

int WCDHit::operator==(const WCDHit& right) const {
    return (this == &right) ? 1 : 0;
}

