#ifndef PMTGLASS_H
#define PMTGLASS_H

#include "PhotoDetector.hh"
#include <G4LogicalVolume.hh>
#include <G4String.hh>
#include <G4Sphere.hh>
#include <G4VSensitiveDetector.hh>

class PMTSpherical : public PhotoDetector {

public:

  PMTSpherical(G4String PMTname,G4double size, G4double mintheta = 0.0*CLHEP::deg, G4double maxtheta = 90.0*CLHEP::deg);
  ~PMTSpherical();
    G4LogicalVolume* GetLogicalVolume() const { return fPhotodetector_log; }
    G4LogicalVolume* GetPhotocathodeLogicalVolume() const { return fPhotocathode_log; }

    // New function to set the sensitive detector
    void SetSensitiveDetector(G4VSensitiveDetector* sd);

  G4double GetRadius() const {return fRadius;}
  G4double GetDiameter() const {return 2.0*fRadius;}

private:
  G4double fRadius;
  G4double fWindowRadius;
  G4double fWindowThickness;
  G4double fPhotocathodeRadius;
  G4double fVacuumRadius;
  G4Material* fWindowMaterial;
  G4double fPhotocathodeThickness;
  G4double fPhotocathodeMinTheta;
  G4double fPhotocathodeMaxTheta;
  G4Material* fPhotocathodeMaterial;

  G4VPhysicalVolume* fWindowInner = nullptr;
  G4VPhysicalVolume* fPhotocathode = nullptr;

  void MakeLogicalVolume();
  void MakePhotocathodeSurface();
};
#endif
