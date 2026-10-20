#include "RunAction.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "RunMessenger.hh"

#include <chrono>

namespace fs = std::filesystem;





RunAction::RunAction() : G4UserRunAction(), fOutputSuffix(""), outputFolder(""){

    fMessenger = new RunMessenger(this);

    //Get the timestamp
    // auto now = std::chrono::system_clock::now();
    // std::time_t t = std::chrono::system_clock::to_time_t(now);
    // std::stringstream ss;
    // ss << std::put_time(std::localtime(&t), "%Y-%m-%d");
    // std::string timestamp = ss.str();

    //Create Directories
    // std::string folder = "data/" + timestamp + "/";
    // fs::create_directories(folder);
    // outputFolder = folder;

    analysisManager = G4AnalysisManager::Instance();

    // Remove old ROOT file
    //fs::remove(outputFolder + "HitsData.root");

    // Create Ntuples for each PMT
    for (int i = 0; i < 7; ++i) {
        std::string ntupleName = "Hits_PMT" + std::to_string(i + 1);
        std::string ntupleTitle = "PMT " + std::to_string(i + 1) + " Hits";
        analysisManager->CreateNtuple(ntupleName.c_str(), ntupleTitle.c_str());
        analysisManager->CreateNtupleIColumn("EventID");      // Integer
        analysisManager->CreateNtupleIColumn("TrackID");      // Integer
        analysisManager->CreateNtupleIColumn("PMTNumber");    // Integer
        analysisManager->CreateNtupleDColumn("HitPositionX"); // Double
        analysisManager->CreateNtupleDColumn("HitPositionY"); // Double
        analysisManager->CreateNtupleDColumn("HitPositionZ"); // Double
        analysisManager->CreateNtupleDColumn("HitTime");      // Double
        analysisManager->CreateNtupleDColumn("Energy");       // Double
        analysisManager->CreateNtupleDColumn("Wavelength");   // Double
        analysisManager->CreateNtupleDColumn("LocalPositionX"); // Double
        analysisManager->CreateNtupleDColumn("LocalPositionY"); // Double
        analysisManager->CreateNtupleDColumn("LocalPositionZ"); // Double
        analysisManager->CreateNtupleDColumn("DirectionX");   // Double
        analysisManager->CreateNtupleDColumn("DirectionY");   // Double
        analysisManager->CreateNtupleDColumn("DirectionZ");   // Double
        analysisManager->CreateNtupleDColumn("PixelPositionX"); // Double
        analysisManager->CreateNtupleDColumn("PixelPositionY"); // Double
        analysisManager->CreateNtupleDColumn("PixelPositionZ"); // Double
        analysisManager->CreateNtupleIColumn("DetectorID");   // Integer
        analysisManager->FinishNtuple(i);
    }
}

RunAction::~RunAction() {
    if (csvFile.is_open()){
        csvFile.close();
    }
    if (totalHitsFile.is_open()) {
        totalHitsFile.close();
    }
    delete fMessenger;
    delete G4AnalysisManager::Instance();
}

void RunAction::BeginOfRunAction(const G4Run*) {
    //Get the timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%Y-%m-%d");
    std::string timestamp = ss.str();

    //Base folder from macro
    if (outputFolder.empty()){
        outputFolder = ".";
    }

    if (outputFolder.back() == '/'){
        outputFolder.pop_back();
    }
    
    //Directories for each tipe of file
    std::string base = outputFolder + "/data/" + timestamp + "/";

    rootFolder = base + "root/";
    csvFolder = base + "csv/";
    totalFolder = base + "total/";

    fs::create_directories(rootFolder);
    fs::create_directories(csvFolder);
    fs::create_directories(totalFolder);

    // Nome base + suffisso
    std::string suffixPart = fOutputSuffix.empty() ? "" : "_" + fOutputSuffix;

    // ROOT file
    std::string rootFile = rootFolder + "HitsData" + suffixPart + ".root";
    fs::remove(rootFile);
    analysisManager->OpenFile(rootFile);

    // CSV file
    std::string csvFileName = csvFolder + "HitsData_PMT" + suffixPart + ".csv";
    fs::remove(csvFileName);
    csvFile.open(csvFileName);

    if (!csvFile.is_open()) {
        G4cerr << "Failed to open CSV file: " << csvFileName << G4endl;
        throw std::runtime_error("Failed to open CSV file: " + csvFileName);
    }
    csvFile << "EventID,PmtID,Hits,TimeHits\n";

    // TotalHits CSV
    std::string totalHitsName = totalFolder + "TotalHitsData" + suffixPart + ".csv";
    fs::remove(totalHitsName);
    totalHitsFile.open(totalHitsName);

    if (!totalHitsFile.is_open()) {
        G4cerr << "Failed to open total hits CSV file" << G4endl;
    } else {
        totalHitsFile << "EventID,TotalHits\n";
    }

    G4cout << ">>> Writing output to files with suffix: " << suffixPart << G4endl;

}

void RunAction::EndOfRunAction(const G4Run*) {
    analysisManager->Write();
    analysisManager->CloseFile();
}

