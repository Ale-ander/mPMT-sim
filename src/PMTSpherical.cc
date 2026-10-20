#include "PMTSpherical.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"

PMTSpherical::PMTSpherical(G4String name, G4double radius, G4double theta1, G4double theta2)
  : PhotoDetector(name),
    fRadius(radius),
    fPhotocathodeMinTheta(theta1),
    fPhotocathodeMaxTheta(theta2)
{
  fWindowRadius = fRadius;
  fWindowThickness = 3.0 * mm;
  fWindowMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Pyrex_Glass");
  fPhotocathodeThickness = 26e-6 * mm;
  fPhotocathodeMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");

  fPhotocathodeRadius = fWindowRadius - fWindowThickness;
  fVacuumRadius = fPhotocathodeRadius;

  MakeLogicalVolume();
  MakePhotocathodeSurface();  // Ensure this is called to set up the logical surface

  CheckParameters();
}

PMTSpherical::~PMTSpherical() {}

void PMTSpherical::MakeLogicalVolume()
{
  G4cout << "PMT diameter is " << G4BestUnit(GetRadius() * 2, "Length") << G4endl;

  G4Material* vacuum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

  G4Sphere* window_solid_1 = new G4Sphere("window_solid_1", 0.0, fWindowRadius, 0.0 * deg, 360.0 * deg, 0.0 * deg, 70 * deg);
  G4Sphere* window_solid_2 = new G4Sphere("window_solid_2", 0.0, fWindowRadius - 0.5 * fWindowThickness, 0.0 * deg, 360.0 * deg, 0.0 * deg, 70.0 * deg);
  G4Sphere* photocathode_solid = new G4Sphere("photocathode_solid", fPhotocathodeRadius - fPhotocathodeThickness, fPhotocathodeRadius, 0.0 * deg, 360.0 * deg, 
                                        	fPhotocathodeMinTheta, fPhotocathodeMaxTheta - fPhotocathodeMinTheta);
  G4Sphere* vacuum_solid = new G4Sphere("vacuum_solid", 0.0, fVacuumRadius, 0.0 * deg, 360.0 * deg, 0.0 * deg, 70 * deg);
  
  G4cout << " photocathode_solid " << fPhotocathodeMinTheta << " fPhotocathodeMaxTheta - fPhotocathodeMinTheta " << fPhotocathodeMaxTheta - fPhotocathodeMinTheta << G4endl;
  
  G4LogicalVolume* vacuum_log = new G4LogicalVolume(vacuum_solid, vacuum, "vacuum_log");

  fPhotodetector_log = new G4LogicalVolume(window_solid_1, fWindowMaterial, "window_log");
  G4LogicalVolume* window_inner_log = new G4LogicalVolume(window_solid_2, fWindowMaterial, "window_inner_log");
  fPhotocathode_log = new G4LogicalVolume(photocathode_solid, fPhotocathodeMaterial, "photocathode_log");

  fWindowInner = new G4PVPlacement(0, G4ThreeVector(), window_inner_log, "Window_innerlayer", fPhotodetector_log, false, 0);
  fPhotocathode = new G4PVPlacement(0, G4ThreeVector(), fPhotocathode_log, "Photocathode", vacuum_log, false, 0);

  new G4PVPlacement(0, G4ThreeVector(), vacuum_log, "Vacuum", window_inner_log, false, 0);
  
  G4cout << "fWindowRadius " << fWindowRadius << " fWindowRadius - 0.5 * fWindowThickness " << fWindowRadius - 0.5 * fWindowThickness << " fVacuumRadius " << fVacuumRadius << " fPhotocathodeRadius " << fPhotocathodeRadius << " fPhotocathodeRadius - fPhotocathodeThickness " << fPhotocathodeRadius - fPhotocathodeThickness << G4endl;
  
  G4VisAttributes* pmtVisAttr = new G4VisAttributes(G4Color(10, 0.73, 0.0));
  pmtVisAttr->SetVisibility(true);
  pmtVisAttr->SetForceSolid(true);
  fPhotodetector_log->SetVisAttributes(pmtVisAttr);

  G4MaterialPropertiesTable* MPT_Pyrex = new G4MaterialPropertiesTable();

  G4double wl[] = {275*nm, 300*nm, 350*nm, 400*nm, 450*nm, 500*nm, 550*nm, 600*nm, 650*nm};
  G4double r[] = {1.53216, 1.513777778, 1.502693878, 1.4955, 1.490567901, 1.48704, 1.484429752, 1.482444444, 1.480899408};

  G4MaterialPropertyVector* mpv = new G4MaterialPropertyVector();
  G4int nEntries = sizeof(wl) / sizeof(G4double);
  G4double energy;
  for (G4int ie = 0; ie < nEntries; ++ie) {
    energy = CLHEP::h_Planck * CLHEP::c_light / wl[ie];
    mpv->InsertValues(energy, r[ie]);
  }
  MPT_Pyrex->AddProperty("RINDEX", mpv);

  G4double wl2[] = {298.9*nm, 299.1*nm, 300.8*nm, 304.5*nm, 310.0*nm, 315.1*nm, 321.9*nm, 330.7*nm, 340.7*nm, 350.7*nm, 361.5*nm, 372.4*nm, 383.6*nm, 395.4*nm, 407.1*nm, 419.3*nm, 432.0*nm, 445.0*nm, 481.2*nm, 650.0*nm};
  G4double al[] = {0*mm, 0*mm, 2.69*mm, 3.73*mm, 4.56*mm, 5.54*mm, 6.91*mm, 8.94*mm, 11.7*mm, 14.7*mm, 18.7*mm, 23.9*mm, 29.4*mm, 46.1*mm, 58.5*mm, 58.5*mm, 58.5*mm, 58.5*mm, 58.5*mm, 58.5*mm, 58.5*mm};

  G4MaterialPropertyVector* mpv2 = new G4MaterialPropertyVector();
  nEntries = sizeof(wl2) / sizeof(G4double);

  for (G4int ie = 0; ie < nEntries; ++ie) {
    energy = CLHEP::h_Planck * CLHEP::c_light / wl2[ie];
    mpv2->InsertValues(energy, 10.0 * al[ie]);
  }

  MPT_Pyrex->AddProperty("ABSLENGTH", mpv2);
  fWindowMaterial->SetMaterialPropertiesTable(MPT_Pyrex);

  if (vacuum == G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic") || vacuum == G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR")) {
    G4MaterialPropertiesTable* MPT_Galactic = new G4MaterialPropertiesTable();

    G4double e[] = {
      2.034*eV, 2.068*eV, 2.103*eV, 2.139*eV,
      2.177*eV, 2.216*eV, 2.256*eV, 2.298*eV,
      2.341*eV, 2.386*eV, 2.433*eV, 2.481*eV,
      2.532*eV, 2.585*eV, 2.640*eV, 2.697*eV,
      2.757*eV, 2.820*eV, 2.885*eV, 2.954*eV,
      3.026*eV, 3.102*eV, 3.181*eV, 3.265*eV,
      3.353*eV, 3.446*eV, 3.545*eV, 3.649*eV,
      3.760*eV, 3.877*eV, 4.002*eV, 4.136*eV
    };

    nEntries = sizeof(e) / sizeof(G4double);

    G4double RINDEX = 1.0;

    G4MaterialPropertyVector* mpv = new G4MaterialPropertyVector();
    for (G4int ie = 0; ie < nEntries; ++ie) {
      mpv->InsertValues(e[ie], RINDEX);
    }

    MPT_Galactic->AddProperty("RINDEX", mpv);
    vacuum->SetMaterialPropertiesTable(MPT_Galactic);
  }
}

void PMTSpherical::MakePhotocathodeSurface()
{
  if (!fPhotocathode_log) {
    G4Exception("PhotoDetector::MakePhotocathodeSurface", "", FatalException, "fPhotocathode_log is NULL!");
  }

  G4cout << "In PMTSpherical::MakePhotocathodeSurface()... " << G4endl;
  G4String name = fPhotocathode_log->GetName() + "OpticalSurface";
  G4cout << "Name of fPhotocathodeLogicalSurface: " << name << G4endl;

  G4OpticalSurface* opSurface = new G4OpticalSurface(name);
  SetPhotocathodeOpticalSurface(opSurface);

  fPhotocathodeLogicalSurface = new G4LogicalBorderSurface(name, fWindowInner, fPhotocathode, opSurface);
}

void PMTSpherical::SetSensitiveDetector(G4VSensitiveDetector* sd)
{
  fPhotocathode_log->SetSensitiveDetector(sd);
}

