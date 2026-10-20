#include "PhotoDetector.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"

PhotoDetector::PhotoDetector(G4String name) :
  fName(name), fQuantumEfficiency(nullptr), fPhotodetector_log(nullptr), fPhotocathode_log(nullptr), fPhotocathodeLogicalSurface(nullptr), fPhotocathodeOpticalSurface(nullptr)
{
    G4cout << "PhotoDetector: Constructor called for " << fName << G4endl;
}

PhotoDetector::~PhotoDetector()
{
    G4cout << "PhotoDetector: Destructor called for " << fName << G4endl;
    delete fQuantumEfficiency;
    delete fPhotodetector_log;
    delete fPhotocathode_log;
}

void PhotoDetector::CheckParameters()
{
    G4cout << "PhotoDetector: CheckParameters called for " << fName << G4endl;

    if (fPhotodetector_log) {
        G4cout << "PhotoDetector: fPhotodetector_log is not NULL for " << fName << G4endl;
    } else {
        G4cout << "PhotoDetector: fPhotodetector_log is NULL for " << fName << G4endl;
    }

    if (fPhotocathode_log) {
        G4cout << "PhotoDetector: fPhotocathode_log is not NULL for " << fName << G4endl;
    } else {
        G4cout << "PhotoDetector: fPhotocathode_log is NULL for " << fName << G4endl;
    }

    if (fQuantumEfficiency) {
        G4cout << "PhotoDetector: fQuantumEfficiency is not NULL for " << fName << G4endl;
    } else {
        G4cout << "PhotoDetector: fQuantumEfficiency is NULL for " << fName << G4endl;
    }

    if (fPhotocathodeLogicalSurface) {
        G4cout << "PhotoDetector: fPhotocathodeLogicalSurface is not NULL for " << fName << G4endl;
    } else {
        G4cout << "PhotoDetector: fPhotocathodeLogicalSurface is NULL for " << fName << G4endl;
    }

    if (fPhotocathodeOpticalSurface) {
        G4cout << "PhotoDetector: fPhotocathodeOpticalSurface is not NULL for " << fName << G4endl;
    } else {
        G4cout << "PhotoDetector: fPhotocathodeOpticalSurface is NULL for " << fName << G4endl;
    }
}

void PhotoDetector::SetPhotocathodeOpticalSurface(G4OpticalSurface* opSurface)
{
    if (!opSurface) {
        G4Exception("PhotoDetector::SetPhotocathodeOpticalSurface", "", FatalException, "Pointer to G4OpticalSurface is NULL!");
    }

    G4cout << "PhotoDetector: Setting optical surface properties for " << fName << G4endl;

    opSurface->SetModel(unified);
    opSurface->SetType(dielectric_metal);
    opSurface->SetFinish(polished);

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
  
   G4double pe[] = { //forr 3 inches PMT
        1.75076 * eV, 1.75691 * eV, 1.76622 * eV, 1.77562 * eV, 1.78195 * eV, 1.79152 * eV, 1.79796 * eV, 1.80445 * eV,
        1.81757 * eV, 1.82419 * eV, 1.83423 * eV, 1.84438 * eV, 1.85464 * eV, 1.86501 * eV, 1.8755 * eV, 1.88967 * eV,
        1.90044 * eV, 1.91867 * eV, 1.93351 * eV, 1.94857 * eV, 1.96774 * eV, 1.98334 * eV, 2.0032 * eV, 2.03169 * eV,
        2.06099 * eV, 2.09993 * eV, 2.14037 * eV, 2.19196 * eV, 2.25116 * eV, 2.31365 * eV, 2.36283 * eV, 2.41416 * eV,
        2.48 * eV, 2.56918 * eV, 2.69372 * eV, 2.81501 * eV, 2.94774 * eV, 3.10321 * eV, 3.26529 * eV, 3.45714 * eV,
        3.67295 * eV, 3.85707 * eV, 3.9329 * eV, 4.01176 * eV, 4.09386 * eV, 4.12765 * eV, 4.17939 * eV, 4.17939 * eV,
        4.19692 * eV, 4.21461 * eV, 4.21461 * eV, 4.23244 * eV, 4.26856 * eV, 4.26856 * eV, 4.36163 * eV
    }; 
    G4double qe[] = { //for 3 inches PMT
        0.000108118, 0.00013466, 0.000172698, 0.000221482, 0.000284047, 0.000364284, 0.000467188, 0.000599159,
        0.000757247, 0.000985471, 0.001245486, 0.001597312, 0.002048522, 0.00262719, 0.00332037, 0.00425831,
        0.005381858, 0.006902129, 0.008851847, 0.011519687, 0.014347613, 0.018400537, 0.023255486, 0.029391404,
        0.036074755, 0.045593014, 0.05596047, 0.068685396, 0.083079051, 0.100489028, 0.121547425, 0.153617495,
        0.185809449, 0.202863255, 0.228060907, 0.2489926, 0.264003795, 0.271845428, 0.267895921, 0.260168215,
        0.241810178, 0.199915956, 0.160512358, 0.130775209, 0.101970506, 0.077216811, 0.046947251, 0.061997315,
        0.036606594, 0.022256539, 0.029824713, 0.017610136, 0.010551278, 0.013731307, 0.008471613
    };  

/*
  G4double pe[] = // photon energy vector, in eV for 8 inches
{
    1.50 * eV, 1.91 * eV, 2.08 * eV, 2.16 * eV, 2.19 * eV, 2.23 * eV,
    2.27 * eV, 2.32 * eV, 2.36 * eV, 2.41 * eV, 2.46 * eV, 2.50 * eV,
    2.56 * eV, 2.61 * eV, 2.67 * eV, 2.72 * eV, 2.79 * eV, 2.85 * eV,
    2.92 * eV, 2.99 * eV, 3.06 * eV, 3.14 * eV, 3.22 * eV, 3.31 * eV,
    3.40 * eV, 3.49 * eV, 3.59 * eV, 3.70 * eV, 3.81 * eV, 3.94 * eV,
    4.07 * eV, 4.20 * eV, 4.60 * eV, 5.00 * eV
};



 G4double qe[] = // quantum efficiency vector for 8 inches
{
    0.00000, 0.00000, 0.03900, 0.05099, 0.05999, 0.07099,
    0.08999, 0.12999, 0.14999, 0.17999, 0.19999, 0.20999,
    0.22999, 0.23999, 0.24999, 0.26999, 0.27999, 0.28999,
    0.29999, 0.31000, 0.31000, 0.31000, 0.31000, 0.31000,
    0.31000, 0.31000, 0.29999, 0.28999, 0.26999, 0.22999,
    0.15999, 0.08999, 0.00999, 0.00000
}; 
*/
/*
G4double qe[] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1
};
*/


    G4int nEntries = sizeof(pe) / sizeof(G4double);
    mpt->AddProperty("EFFICIENCY", pe, qe, nEntries);

    G4double RINDEX = 2.9;
    G4double KINDEX = 1.6;
    G4double* rIndexArray = new G4double[nEntries];
    G4double* kIndexArray = new G4double[nEntries];
    std::fill_n(rIndexArray, nEntries, RINDEX);
    std::fill_n(kIndexArray, nEntries, KINDEX);

    mpt->AddProperty("REALRINDEX", pe, rIndexArray, nEntries);
    mpt->AddProperty("IMAGINARYRINDEX", pe, kIndexArray, nEntries);

    opSurface->SetMaterialPropertiesTable(mpt);

    fQuantumEfficiency = new G4MaterialPropertyVector(*mpt->GetProperty("EFFICIENCY"));

    fPhotocathodeOpticalSurface = opSurface;

    if (fPhotocathode_log) {
        G4String name = fPhotocathode_log->GetName();
        G4cout << "PhotoDetector: Creating optical surface for " << name << G4endl;
        fPhotocathodeLogicalSurface = new G4LogicalSkinSurface(name, fPhotocathode_log, fPhotocathodeOpticalSurface);
    } else {
        G4Exception("PhotoDetector::SetPhotocathodeOpticalSurface", "", FatalException, "fPhotocathode_log is NULL!");
    }

    CheckParameters();
}

