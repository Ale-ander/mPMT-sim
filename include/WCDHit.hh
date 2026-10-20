#ifndef WCDHIT_HH
#define WCDHIT_HH

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4UnitsTable.hh"

class WCDHit : public G4VHit {
public:
    WCDHit();
    virtual ~WCDHit();
    WCDHit(const WCDHit& right);
    const WCDHit& operator=(const WCDHit& right);
    int operator==(const WCDHit& right) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    void SetTrackID(G4int id) { fTrackID = id; }
    void SetPMTNumber(G4int number) { fPMTNumber = number; }
    void SetHitPosition(G4ThreeVector pos) { fHitPosition = pos; }
    void SetHitTime(G4double time) { fHitTime = time; }
    void SetEnergy(G4double energy) { fEnergy = energy; }
    void SetWavelength(G4double wavelength) { fWavelength = wavelength; }
    void SetLocalPosition(G4ThreeVector pos) { fLocalPosition = pos; }
    void SetDirection(G4ThreeVector dir) { fDirection = dir; }
    void SetPixelPosition(G4ThreeVector pos) { fPixelPosition = pos; }
    void SetDetectorID(G4int id) { fDetectorID = id; }

    G4int GetTrackID() const { return fTrackID; }
    G4int GetPMTNumber() const { return fPMTNumber; }
    G4ThreeVector GetHitPosition() const { return fHitPosition; }
    G4double GetHitTime() const { return fHitTime; }
    G4double GetEnergy() const { return fEnergy; }
    G4double GetWavelength() const { return fWavelength; }
    G4ThreeVector GetLocalPosition() const { return fLocalPosition; }
    G4ThreeVector GetDirection() const { return fDirection; }
    G4ThreeVector GetPixelPosition() const { return fPixelPosition; }
    G4int GetDetectorID() const { return fDetectorID; }

private:
    G4int fTrackID;
    G4int fPMTNumber;
    G4ThreeVector fHitPosition;
    G4double fHitTime;
    G4double fEnergy;
    G4double fWavelength;
    G4ThreeVector fLocalPosition;
    G4ThreeVector fDirection;
    G4ThreeVector fPixelPosition;
    G4int fDetectorID;
};

typedef G4THitsCollection<WCDHit> WCDHitsCollection;

extern G4Allocator<WCDHit> WCDHitAllocator;

inline void* WCDHit::operator new(size_t) {
    void* hit;
    hit = (void*)WCDHitAllocator.MallocSingle();
    return hit;
}

inline void WCDHit::operator delete(void* hit) {
    WCDHitAllocator.FreeSingle((WCDHit*)hit);
}

#endif

