#ifndef PHOTODETECTOR_H
#define PHOTODETECTOR_H

#include "globals.hh"
#include <G4MaterialPropertyVector.hh>
#include <G4String.hh>
#include <G4LogicalVolume.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSurface.hh>


class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;
class G4OpticalSurface;
class G4LogicalSurface;

class PhotoDetector {

public:
  PhotoDetector(G4String name);
  virtual ~PhotoDetector();

  G4String GetName() const { return fName; }
  G4LogicalVolume* GetLogicalVolume() const { return fPhotodetector_log; }
  G4LogicalVolume* GetPhotocathodeLogicalVolume() const { return fPhotocathode_log; }
  void SetPhotocathodeOpticalSurface(G4OpticalSurface*);
  const G4MaterialPropertyVector* GetQuantumEfficiency(void) const { return fQuantumEfficiency; }
  G4double GetQuantumEfficiency(const G4double& energy) const { return fQuantumEfficiency->Value(energy); }

  virtual G4double GetRadius() const = 0;

protected:
  G4String fName;
  G4LogicalVolume* fPhotodetector_log = nullptr;
  G4LogicalVolume* fPhotocathode_log = nullptr;

  G4LogicalSurface* fPhotocathodeLogicalSurface = nullptr;
  G4OpticalSurface* fPhotocathodeOpticalSurface = nullptr;
  G4MaterialPropertyVector* fQuantumEfficiency = nullptr;

  virtual void MakeLogicalVolume() = 0;
  virtual void CheckParameters();
};

#endif

