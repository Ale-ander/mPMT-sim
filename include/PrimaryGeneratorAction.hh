#ifndef PRIMARY_GENERATOR_ACTION_HH
#define PRIMARY_GENERATOR_ACTION_HH
#include <G4GeneralParticleSource.hh> // Include the header file for GPS
#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4ParticleGun.hh>
#include <G4GenericMessenger.hh>
#include <G4SystemOfUnits.hh>
#include <map>
#include <vector>

// class G4ParticleGun;
struct CorsikaPart { int pdg; double x, y, t, px, py, pz; };
// Task 2b.1 Include the proper header file or forward-declare the class for GPS

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction();
    void GeneratePrimaries(G4Event* anEvent) override;

private:
	// Task 2b.1: Replace the gun with a GPS instance called fGPS;
    // G4GeneralParticleSource* fGPS;
  //  G4ParticleGun* fGun;
  // G4ParticlefGun* fGPS;

    void LoadFile();

    G4ParticleGun*       fGun;
    G4GenericMessenger*  fMessenger;
    G4String             fFileName = "shower.csv";
    G4double             fZStart   = 1.7 * m;   // z di partenza delle particelle
    G4double             fRMax     = 50.0 * m;   // raggio massimo (piano xy) entro cui inserire le particelle
    bool                 fLoaded   = false;
    std::map<int, std::vector<CorsikaPart>> fShowers;   // chiave: numero sciame CORSIKA (da 1)
};

#endif
