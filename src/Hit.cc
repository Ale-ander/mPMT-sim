#include "Hit.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4Circle.hh"
#include "G4VisAttributes.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

G4Allocator<Hit> HitAllocator;

Hit::Hit()
    : fTrackID(-1), fPMTNumber(-1), fHitPosition(G4ThreeVector()), fHitTime(0.),
      fEnergy(0.), fWavelength(0.), fLocalPosition(G4ThreeVector()), fDirection(G4ThreeVector()),
      fPixelPosition(G4ThreeVector()), fDetectorID(-1) {}

Hit::~Hit() {}

Hit::Hit(const Hit& right)
    : G4VHit(), fTrackID(right.fTrackID), fPMTNumber(right.fPMTNumber),
      fHitPosition(right.fHitPosition), fHitTime(right.fHitTime),
      fEnergy(right.fEnergy), fWavelength(right.fWavelength),
      fLocalPosition(right.fLocalPosition), fDirection(right.fDirection),
      fPixelPosition(right.fPixelPosition), fDetectorID(right.fDetectorID) {}

const Hit& Hit::operator=(const Hit& right) {
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

int Hit::operator==(const Hit& right) const {
    return (this == &right) ? 1 : 0;
}

