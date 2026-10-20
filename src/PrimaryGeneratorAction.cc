#include "PrimaryGeneratorAction.hh"

#include <G4ParticleTable.hh>
#include <G4Event.hh>
#include <G4SystemOfUnits.hh>
#include <G4ThreeVector.hh>
#include <G4Exception.hh>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
    fGun = new G4ParticleGun(1);

    // comandi macro: /corsika/file, /corsika/zStart, /corsika/rMax
    fMessenger = new G4GenericMessenger(this, "/corsika/", "CORSIKA input");
    fMessenger->DeclareProperty("file", fFileName, "CSV file with the CORSIKA particles");
    fMessenger->DeclarePropertyWithUnit("zStart", "m", fZStart, "z where the particles are started");
    fMessenger->DeclarePropertyWithUnit("rMax",   "m", fRMax,   "particles with r > rMax are skipped");
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fGun;
    delete fMessenger;
}

void PrimaryGeneratorAction::LoadFile()
{
    ifstream in(fFileName);
    if (!in) {
        G4Exception("PrimaryGeneratorAction::LoadFile", "Corsika001", FatalException,
                    ("Cannot open " + fFileName).c_str());
    }
    string line;
    getline(in, line);                                  // header
    while (getline(in, line)) {
        stringstream ss(line);
        string tok;
        vector<double> v;
        while (getline(ss, tok, ',')) v.push_back(stod(tok));
        if (v.size() < 8) continue;
        // event,pdg,x_cm,y_cm,t_ns,px,py,pz
        fShowers[(int)v[0]].push_back({(int)v[1], v[2], v[3], v[4], v[5], v[6], v[7]});
    }
    G4cout << "Loaded " << fShowers.size() << " showers from " << fFileName << G4endl;
    fLoaded = true;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    // Carico al primo evento, così la macro ha già impostato il nome del file
    if (!fLoaded) LoadFile();

    const int id = anEvent->GetEventID() + 1;           // Geant4 parte da 0, CORSIKA da 1
    auto it = fShowers.find(id);
    if (it == fShowers.end()) {
        G4Exception("PrimaryGeneratorAction::GeneratePrimaries", "Corsika002", JustWarning,
                    "No CORSIKA shower for this event: beamOn is larger than the number of showers in the file");
        return;
    }

    auto table = G4ParticleTable::GetParticleTable();
    int nIn = 0, nOut = 0;

    for (const auto& p : it->second) {
        auto def = table->FindParticle(p.pdg);
        if (!def) continue;

        const G4double x = p.x * cm;
        const G4double y = p.y * cm;
        if (std::hypot(x, y) > fRMax) { ++nOut; continue; }   // fuori dal volume utile

        const G4ThreeVector mom(p.px * GeV, p.py * GeV, p.pz * GeV);
        const G4double pmag = mom.mag();
        if (pmag <= 0.) continue;
        const G4double m    = def->GetPDGMass();
        const G4double ekin = std::sqrt(pmag * pmag + m * m) - m;

        fGun->SetParticleDefinition(def);
        fGun->SetParticleMomentumDirection(mom.unit());
        fGun->SetParticleEnergy(ekin);
        fGun->SetParticlePosition(G4ThreeVector(x, y, fZStart));
        fGun->SetParticleTime(p.t * ns);
        fGun->GeneratePrimaryVertex(anEvent);
        ++nIn;
    }

    G4cout << "Event " << anEvent->GetEventID() + 1 << " (CORSIKA shower " << id << "): "
           << nIn << " particles injected, " << nOut << " skipped (r > "
           << fRMax / m << " m)" << G4endl;
}