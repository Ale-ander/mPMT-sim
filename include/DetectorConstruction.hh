#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include <G4SystemOfUnits.hh>
#include <G4VisAttributes.hh>
#include <G4Tubs.hh>
#include <G4Sphere.hh>
#include <G4Box.hh>
#include <G4SubtractionSolid.hh>
#include <G4MaterialPropertiesTable.hh>
#include <G4OpticalPhoton.hh>
#include <G4SDManager.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4OpticalParameters.hh>
#include "G4UnitsTable.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include <G4VUserDetectorConstruction.hh>
#include <vector>
#include "globals.hh"

class G4LogicalVolume;
class PMTSpherical;
class G4VSensitiveDetector;

struct PMT_coord {
    G4double theta_deg;
    G4double phi_deg;
};

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct() override;
    virtual void ConstructSDandField() override;

    const std::vector<PMTSpherical*>& GetPhotoDetectors() const { return fPhotoDetectors; }

    std::vector<G4ThreeVector> calcolaPosizioniXYZ(G4double r, const std::vector<PMT_coord>& coordinates, G4double zoffset);
    void ConstructMaterials();

private:
    std::vector<PMTSpherical*> fPhotoDetectors;

};

#endif

