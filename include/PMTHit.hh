#ifndef PMTHIT_HH
#define PMTHIT_HH

#include <G4VHit.hh>
#include <G4ThreeVector.hh>
#include <G4Allocator.hh>
#include <CLHEP/Units/PhysicalConstants.h>  // Include CLHEP constants
class PMTHit : public G4VHit {
public:
    PMTHit();
    virtual ~PMTHit();
    PMTHit(const PMTHit& right);
    const PMTHit& operator=(const PMTHit& right);
    int operator==(const PMTHit& right) const;

    inline void* operator new(size_t);
    inline void operator delete(void*);

    void SetTrackID(G4int id) { fTrackID = id; }
    void SetPMTNumber(G4int num) { fPMTNumber = num; }
    void SetHitPosition(const G4ThreeVector& pos) { fHitPosition = pos; }
    void SetHitTime(G4double time) { fHitTime = time; }
    void SetEnergy(G4double energy) { fEnergy = energy; }
    void SetWavelength(G4double wavelength) { fWavelength = wavelength; }
    void SetLocalPosition(const G4ThreeVector& pos) { fLocalPosition = pos; }
    void SetDirection(const G4ThreeVector& dir) { fDirection = dir; }
    void SetPixelPosition(const G4ThreeVector& pos) { fPixelPosition = pos; }
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

extern G4Allocator<PMTHit> PMTHitAllocator;

inline void* PMTHit::operator new(size_t) {
    return (void*) PMTHitAllocator.MallocSingle();
}

inline void PMTHit::operator delete(void* hit) {
    PMTHitAllocator.FreeSingle((PMTHit*) hit);
}

#endif

