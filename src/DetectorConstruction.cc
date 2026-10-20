#include "DetectorConstruction.hh"
#include "PMTSpherical.hh"
#include "PMTSD.hh"
//#include "WCDSD.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4VisAttributes.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4Sphere.hh>
#include <G4Cons.hh>
#include <G4SubtractionSolid.hh>
#include <G4MaterialPropertiesTable.hh>
#include <G4OpticalPhoton.hh>
#include <G4SDManager.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4OpticalParameters.hh>
#include "G4UnitsTable.hh"

DetectorConstruction::DetectorConstruction(): G4VUserDetectorConstruction(){
}


DetectorConstruction::~DetectorConstruction() {    
}

void DetectorConstruction::ConstructSDandField() {
    // No sensitive detectors or fields are defined here by default.
}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    G4NistManager* nist = G4NistManager::Instance();

    
    ConstructMaterials();

    ////////////////////////
    // TANK CONFIGURATION //
    ////////////////////////

    G4double worldHeight = 10 * m;
    G4double worldRadius = 8 * m;
    G4double TankHeight = 2 * m;
    G4double TankRadius = 1.5 * m;
    G4double TankThickness = 0.1 * m;
    
    // World volume
    G4VSolid* worldCylinder = new G4Tubs("world", 0, worldRadius, worldHeight / 2, 0, 360 * deg);
    G4LogicalVolume* worldLog = new G4LogicalVolume(worldCylinder, nist->FindOrBuildMaterial("G4_AIR"), "world");
    G4VisAttributes* visAttr = new G4VisAttributes();
    visAttr->SetVisibility(false);
    worldLog->SetVisAttributes(visAttr);
    G4VPhysicalVolume* worldPhys = new G4PVPlacement(nullptr, {}, worldLog, "world", nullptr, false, 0);

    // Water cylinder
    G4VSolid* waterCylinder = new G4Tubs("waterCylinder", 0, TankRadius, TankHeight / 2, 0, 360 * deg);
    G4Material* water = nist->FindOrBuildMaterial("G4_WATER");
    G4LogicalVolume* waterLog = new G4LogicalVolume(waterCylinder, water, "waterCylinder");
    G4VisAttributes* waterVisAttr = new G4VisAttributes(G4Colour(0, 0, 1)); // Blue color for water
    waterVisAttr->SetVisibility(true);
    waterLog->SetVisAttributes(waterVisAttr);

    // Placement of the water cylinder at the center of the world volume
    G4ThreeVector waterPosition(0, 0, 0);
    G4VPhysicalVolume* waterPhys = new G4PVPlacement(0, waterPosition, waterLog, "waterCylinder", worldLog, false, 0, true);

    //Tyvek surface
    G4OpticalSurface* tyvekOpSurface = new G4OpticalSurface("TyvekSurface");
    G4LogicalBorderSurface* tyvekSurface = new G4LogicalBorderSurface("TyvekSurface", waterPhys, worldPhys, tyvekOpSurface);
    
    tyvekOpSurface->SetType(dielectric_dielectric);
    tyvekOpSurface->SetFinish(groundfrontpainted);
    tyvekOpSurface->SetModel(unified);
    tyvekOpSurface->SetSigmaAlpha(0.17);

    ////////////////////////
    // OPTICAL PROPERTIES //
    ////////////////////////

    // Adding optical properties to WATER
    // Refractive Index
    G4double photonEnergy[] = 
        { 1.91 * eV, 2.07 * eV, 2.25 * eV, 2.48 * eV, 2.76 * eV,
          3.10 * eV, 3.54 * eV, 4.13 * eV, 4.51 * eV };
    G4double refractiveIndex[] =
        { 1.33165, 1.33293, 1.33458, 1.33676,
          1.3397, 1.34378, 1.34978, 1.35942, 1.37761 };

    //Absorption lenght
    G4double e[] =
      {2.08*eV, 2.16*eV, 2.19*eV, 2.23*eV, 2.27*eV, 2.32*eV,
       2.36*eV, 2.41*eV, 2.46*eV, 2.50*eV, 2.56*eV, 2.61*eV,
       2.67*eV, 2.72*eV, 2.79*eV, 2.85*eV, 2.92*eV, 2.99*eV,
       3.06*eV, 3.14*eV, 3.22*eV, 3.31*eV, 3.40*eV, 3.49*eV,
       3.59*eV, 3.70*eV, 3.81*eV, 3.94*eV, 4.07*eV, 4.20*eV};

    G4double water_al[] =
      {0.092, 0.133, 0.180, 0.203, 0.226, 0.258,
       0.284, 0.302, 0.403, 0.560, 0.735, 0.818,
       0.923, 0.923, 0.993, 0.993, 1.000, 0.941,
       0.889, 0.842, 0.754, 0.655, 0.480, 0.380,
       0.311, 0.257, 0.212, 0.171, 0.137, 0.102};
    
     // Scale absorption length by 100m
    for (G4int ie=0; ie<sizeof(e)/sizeof(G4double); ++ie){
      water_al[ie] *= 100.0*m;
    }

    G4MaterialPropertiesTable* waterMPT = new G4MaterialPropertiesTable();
    waterMPT->AddProperty("RINDEX", photonEnergy, refractiveIndex, sizeof(photonEnergy)/sizeof(G4double));
    waterMPT->AddProperty("ABSLENGTH", e, water_al, sizeof(e)/sizeof(G4double));
    
    water->SetMaterialPropertiesTable(waterMPT);
    
    //Adding optical properties to TYVEK (reflectivity from Auger  <liner id="1073B-tyvek">)
    G4MaterialPropertiesTable* Tyvek_MPT = new G4MaterialPropertiesTable();

    G4double Tyvek_e[] =
      {2.08*eV, 2.16*eV, 2.19*eV, 2.23*eV, 2.27*eV, 2.32*eV,
       2.36*eV, 2.41*eV, 2.46*eV, 2.50*eV, 2.56*eV, 2.61*eV,
       2.67*eV, 2.72*eV, 2.79*eV, 2.85*eV, 2.92*eV, 2.99*eV,
       3.06*eV, 3.14*eV, 3.22*eV, 3.31*eV, 3.40*eV, 3.49*eV,
       3.59*eV, 3.70*eV, 3.81*eV, 3.94*eV, 4.07*eV, 4.20*eV};

    G4double Tyvek_r[] =
      {0.9957, 0.9953, 0.9948, 0.9942, 0.9939, 0.9937,
       0.9937, 0.9940, 0.9945, 0.9954, 0.9964, 0.9975,
       0.9985, 0.9993, 1.0000, 1.0000, 0.9993, 0.9977,
       0.9948, 0.9903, 0.9840, 0.9753, 0.9642, 0.9500,
       0.9334, 0.9108, 0.8849, 0.8541, 0.8178, 0.7755};
    

    // Scale tyvek reflectivity
    // const G4double SCALE = 0.94; // <- real one
    const G4double SCALE = 0.0; 
    // const G4double SCALE = 0.2;
    for (G4int ie=0; ie<sizeof(Tyvek_e)/sizeof(G4double); ie++) {
        Tyvek_r[ie] *= SCALE;
    }
    Tyvek_MPT->AddProperty("REFLECTIVITY",Tyvek_e,Tyvek_r,sizeof(Tyvek_e)/sizeof(G4double));
    

    //    <specularLobe> <!-- fraction of light reflected in a lobe with width characterized by sigmaAlpha -->
    G4double eSpecLobe[] = {2.08*eV, 3.0*eV, 4.20*eV};
    G4double fracSpecLobe[] = {0.2, 0.2, 0.2};
    Tyvek_MPT->AddProperty("SPECULARLOBECONSTANT",eSpecLobe,fracSpecLobe,G4int(sizeof(eSpecLobe)/sizeof(G4double)));

    //  <specularSpike> <!-- fraction of light reflected as in a mirror -->
    G4double eSpecSpike[] = {2.08*eV, 3.0*eV, 4.20*eV};
    G4double fracSpecSpike[] = {0.0, 0.0, 0.0};
    Tyvek_MPT->AddProperty("SPECULARSPIKECONSTANT",eSpecSpike,fracSpecSpike,G4int(sizeof(eSpecSpike)/sizeof(G4double)));
   
    //  <backscatter> <!-- fraction of backscattered light  -->
    G4double eBackScat[] = {2.08*eV, 3.0*eV, 4.20*eV};
    G4double fracBackScat[] = {0, 0, 0};
    Tyvek_MPT->AddProperty("BACKSCATTERCONSTANT",eBackScat,fracBackScat,G4int(sizeof(eBackScat)/sizeof(G4double))); 
   
    tyvekOpSurface->SetMaterialPropertiesTable(Tyvek_MPT);

    /////////////////////////////////////////
    // POM-C (Polyoxymethylene) definition //
    /////////////////////////////////////////
    G4Element* elC = nist->FindOrBuildElement("C");
    G4Element* elH = nist->FindOrBuildElement("H");
    G4Element* elO = nist->FindOrBuildElement("O");

    G4double density = 1.41 * g/cm3;
    G4Material* pomC = new G4Material("POM-C", density, 3);

    // POM_C is CH2O
    pomC->AddElement(elC, 1);
    pomC->AddElement(elH, 2);
    pomC->AddElement(elO, 1);

    /////////////////
    // ACRYLIC DOME//
    /////////////////
    G4double dome_height = 9.7 * cm;
    G4double dome_r_base = (48.5 / 2.0) * cm;
    G4double dome_thickness = 1.3 * cm;
    G4double dome_outer_r = (dome_r_base * dome_r_base + dome_height * dome_height) / (2.0 * dome_height);
    G4double dome_inner_r = dome_outer_r - dome_thickness;
    G4double thetaMax = std::acos((dome_outer_r - dome_height) / dome_outer_r);
    G4Sphere* dome = new G4Sphere("Dome", dome_inner_r, dome_outer_r, 0.0 * deg, 360.0 * deg, 0.0 * deg, thetaMax);
    G4LogicalVolume* logicalDome = new G4LogicalVolume(dome, nist->FindOrBuildMaterial("G4_PLEXIGLASS"), "Dome");
    G4VPhysicalVolume* domePhys = new G4PVPlacement(0, G4ThreeVector(0,0,-TankHeight/2-dome_inner_r/10*cm+18*cm+dome_height), logicalDome, "Dome", waterLog, false, 0, false);
    // G4VisAttributes* domeVisAttr = new G4VisAttributes(G4Color(0.0, 0.0, 1.0));
    // domeVisAttr->SetVisibility(true);
    // domeVisAttr->SetForceSolid(false);
    // logicalDome->SetVisAttributes(domeVisAttr);

    /////////
    // GEL //
    /////////
    G4double gel_r = dome_inner_r/10 * cm; // Gel radius is slightly smaller than the inner radius of the dome
    G4Sphere* gel = new G4Sphere("Gel", 0, gel_r, 0, 360*deg, 0, 90*deg);
    G4Box* solidCutOffContainer = new G4Box("cutOffContainer",dome_inner_r/10 * cm,dome_inner_r/10 * cm,(dome_inner_r-dome_height)/10 * cm);
    G4SubtractionSolid* mPMT_top_cap_container = new G4SubtractionSolid("gel_cut",gel,solidCutOffContainer); 
    G4LogicalVolume* logicalGel = new G4LogicalVolume(mPMT_top_cap_container, nist->FindOrBuildMaterial("SilGel"), "Gel");
    G4VPhysicalVolume* gelPhys = new G4PVPlacement(0, G4ThreeVector(0,0,-TankHeight/2-gel_r/10*cm+18*cm+dome_height), logicalGel, "Gel", waterLog, false, 0, false);
    // G4VisAttributes* gelVisAttr = new G4VisAttributes(G4Color(0.0, 0.0, 1.0));
    // gelVisAttr->SetVisibility(true);
    // gelVisAttr->SetForceSolid(false);
    // logicalGel->SetVisAttributes(gelVisAttr);

    ////////////////////
    // POM-C CYLINDER //
    ////////////////////
    G4double cylinder_outer_d = 51.4 * cm;  
    G4double cylinder_inner_d = 45 * cm;
    G4double cylinder_height = 15 * cm;
    G4Tubs* cylinder = new G4Tubs("Cylinder", cylinder_inner_d/2, cylinder_outer_d/2, cylinder_height / 2, 0.0 *deg, 360.0 * deg);
    G4LogicalVolume* logicalCylinder = new G4LogicalVolume(cylinder, pomC, "Cylinder"); // come in wcsim blacksheet = polietilene
    G4VPhysicalVolume* cylinderPhys = new G4PVPlacement(0, G4ThreeVector(0,0,-TankHeight/2+cylinder_height/2+3.0*cm), logicalCylinder, "Cylinder", waterLog, false, 0, false);
    G4VisAttributes* cylinderVisAttr = new G4VisAttributes(G4Color(0.0, 0.0, 0.0));
    cylinderVisAttr->SetVisibility(true);
    cylinderVisAttr->SetForceSolid(true);
    logicalCylinder->SetVisAttributes(cylinderVisAttr);
    
    ///////////////////////////////
    // STAINLESS STEEL BACKPLATE //
    ///////////////////////////////
    G4double backplate_outer_d = 59 * cm;  
    G4double backplate_inner_d = 0 * cm;
    G4double backplate_height = 3 * cm;
    G4Tubs* backplate = new G4Tubs("Backplate", backplate_inner_d/2, backplate_outer_d/2, backplate_height / 2, 0.0 *deg, 360.0 * deg);
    G4LogicalVolume* logicalBackplate = new G4LogicalVolume(backplate, nist->FindOrBuildMaterial("G4_STAINLESS-STEEL"), "Backplate");
    G4VPhysicalVolume* backplatePhys = new G4PVPlacement(0, G4ThreeVector(0,0,-TankHeight/2+backplate_height/2), logicalBackplate, "Backplate", waterLog, false, 0, false);
    G4VisAttributes* backplateVisAttr = new G4VisAttributes(G4Color(0.45, 0.45, 0.45));
    backplateVisAttr->SetVisibility(true);
    backplateVisAttr->SetForceSolid(true);
    logicalBackplate->SetVisAttributes(backplateVisAttr);

    //////////
    // PMTs //
    //////////
    G4double pmtRadius = 80 * mm / 2;
    G4cout << "pmtRadius = " << G4BestUnit(pmtRadius, "Length") << G4endl;

    // PMT angular coordinates (theta, phi) in radians
    std::vector<PMT_coord> PMT_theta_phi = {
        {34.0,   0.0}, // PMT 1
        {34.0, 330.0}, // PMT 2
        {34.0, 300.0}, // PMT 3
        {34.0, 270.0}, // PMT 4
        {34.0, 240.0}, // PMT 5
        {34.0, 210.0}, // PMT 6
        {34.0, 180.0}, // PMT 7
        {34.0, 150.0}, // PMT 8
        {34.0, 120.0}, // PMT 9
        {34.0,  90.0}, // PMT 10
        {34.0,  60.0}, // PMT 11
        {34.0,  30.0}, // PMT 12
        {17.0,   0.0}, // PMT 13
        {17.0, 300.0}, // PMT 14
        {17.0, 240.0}, // PMT 15
        {17.0, 180.0}, // PMT 16
        {17.0, 120.0}, // PMT 17
        {17.0,  60.0}, // PMT 18
        { 0.0,   0.0}  // PMT 19
    };

    G4double radius = 0.3; // PMT matrix radius in meters <------- TODO: find the real value
    G4cout << "PMTs radius: " << G4BestUnit(radius, "Length") << G4endl;
    G4double pmt_z_offset = 0.01; // Offset for PMT placement along the z-axis in meters
    G4cout << "PMTs shift: " << G4BestUnit(pmt_z_offset, "Length") << G4endl;
    std::vector<G4ThreeVector> pmtPositions = calcolaPosizioniXYZ(radius, PMT_theta_phi, pmt_z_offset); // tank height = 2m

    std::vector<G4RotationMatrix*> pmtRotations;
    pmtRotations.reserve(PMT_theta_phi.size());

    for (size_t i = 0; i < PMT_theta_phi.size(); ++i) {
        G4RotationMatrix* rot = new G4RotationMatrix();
        rot->rotateY(PMT_theta_phi[i].theta_deg * deg);
        rot->rotateZ(PMT_theta_phi[i].phi_deg * deg);
        pmtRotations.push_back(rot);
    }

    for (size_t i = 0; i < pmtPositions.size(); ++i) {
        G4cout << "PMT " << i + 1 << " position: " << G4BestUnit(pmtPositions[i].x(), "Length") << ", " << G4BestUnit(pmtPositions[i].y(), "Length") << ", " << G4BestUnit(pmtPositions[i].z(), "Length") << " rotation: " << *pmtRotations[i] << G4endl;
    }

    G4String PMTSDname = "PMTSensitiveDetector";
    G4VSensitiveDetector* pmtSD = new PMTSD(PMTSDname);
    G4SDManager::GetSDMpointer()->AddNewDetector(pmtSD);

    for (size_t i = 0; i < pmtPositions.size(); ++i) {
        PMTSpherical* pmt = new PMTSpherical("PMT", pmtRadius, 0 * deg, 42.1 * deg);
        G4LogicalVolume* pmtLog = pmt->GetLogicalVolume();
        fPhotoDetectors.push_back(pmt); // Store the PMT in the list of photo detectors
        G4Transform3D transform(*pmtRotations[i], pmtPositions[i]);
        new G4PVPlacement(transform, pmtLog, "PMT", logicalGel, false, i, false);
        pmt->SetSensitiveDetector(pmtSD);  // Set the sensitive detector for the PMT
    }

    /////////////////////
    // REFLECTOR RINGS //
    /////////////////////
    G4Material* myReflectorMaterial = nist->FindOrBuildMaterial("G4_Al");
    G4double reflectorphotonEnergy[] = { 1.5*eV, 6.0*eV };
    const G4int nEntries = sizeof(reflectorphotonEnergy) / sizeof(G4double);
    G4double reflectivity[] = { 0.9999, 0.9999 };
    G4MaterialPropertiesTable* reflectorSurfaceProperty = new G4MaterialPropertiesTable();
    reflectorSurfaceProperty->AddProperty("REFLECTIVITY", reflectorphotonEnergy, reflectivity, nEntries);
    G4OpticalSurface* reflectorOpticalSurface = new G4OpticalSurface("ReflectorOpticalSurface");
    reflectorOpticalSurface->SetModel(glisur);            // mirror
    reflectorOpticalSurface->SetType(dielectric_metal);
    reflectorOpticalSurface->SetFinish(polished);
    reflectorOpticalSurface->SetMaterialPropertiesTable(reflectorSurfaceProperty);

    G4double pRmin1 = 36.64 * mm; 
    G4double pRmin2 = 45.30 * mm; 
    G4double thickness = 1.0 * mm;
    G4double height = 8.31 * mm;
    G4double pDz    = height / 2.0;
    G4double pRmax1 = pRmin1 + thickness;
    G4double pRmax2 = pRmin2 + thickness;
    G4double zPMTborder = pmtRadius * std::cos(70 * deg);
    G4double localOffsetZ = pDz + zPMTborder;
    G4ThreeVector localOffset(0, 0, localOffsetZ);
    G4Cons* reflectorSolid = new G4Cons("ReflectorRing_Solid",pRmin1,pRmax1,pRmin2,pRmax2,pDz,0.0 * deg,360.0 * deg);
    G4LogicalVolume* reflectorLog = new G4LogicalVolume(reflectorSolid,nist->FindOrBuildMaterial("G4_Al"),"ReflectorRing_Logical");

    for (size_t i = 0; i < pmtPositions.size(); ++i) {
        G4ThreeVector globalOffset = (*pmtRotations[i]) * localOffset;
        G4ThreeVector reflectorPosition = pmtPositions[i] + globalOffset;
        G4Transform3D reflectorTransform(*pmtRotations[i], reflectorPosition);
        new G4PVPlacement(reflectorTransform, reflectorLog, "ReflectorRing", logicalGel, false, i, false);
    }
    new G4LogicalSkinSurface("ReflectorSkinSurface", reflectorLog, reflectorOpticalSurface);
    G4VisAttributes* reflectorVisAttr = new G4VisAttributes(G4Color(0.87, 0.87, 0.87));
    reflectorVisAttr->SetVisibility(true);
    reflectorVisAttr->SetForceSolid(true);
    reflectorLog->SetVisAttributes(reflectorVisAttr);

    return worldPhys;
}


// ============================================================================
// FUNZIONE: Calcola le posizioni Cartesiane (X, Y, Z) a partire da (Theta, Phi)
// ============================================================================
 std::vector<G4ThreeVector> DetectorConstruction::calcolaPosizioniXYZ(G4double r, const std::vector<PMT_coord>& coordinates, G4double zoffset) {
    std::vector<G4ThreeVector> posizioni;
    posizioni.reserve(coordinates.size());

    const G4double degToRad = M_PI / 180.0;

    for (const auto& pmt : coordinates) {
        G4double theta = pmt.theta_deg * degToRad;
        G4double phi   = pmt.phi_deg * degToRad;

        G4double x = r * std::sin(theta) * std::cos(phi);
        G4double y = r * std::sin(theta) * std::sin(phi);
        G4double z = r * std::cos(theta);

        posizioni.push_back({x*m, y*m, (z-zoffset)*m});
    }

    return posizioni;
}
