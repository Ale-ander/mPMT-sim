#include "PMTHamamatsu14374_v0.hh"
#include "DetectorConstruction.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"


PMTHamamatsu14374_v0::PMTHamamatsu14374_v0(G4String name, G4double radius) :
    PMTSpherical(name,radius,0.0*deg,42.1*deg)
{;}


PMTHamamatsu14374_v0::~PMTHamamatsu14374_v0()
{;}
