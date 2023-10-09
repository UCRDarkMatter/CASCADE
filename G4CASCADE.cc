/********************************************************************************
 *  G4CASCADE: Geant4 Extension for Neutron Capture Gamma De-Excitation    *
 *                                                                              *
 *  Description:                                                                *
 *  G4CASCADE is a Geant4 extension designed to handle the de-excitation   *
 *  of a nucleus excited by a neutron capture event. It utilizes basic level    *
 *  structure data from the CapGam library to generate gamma energies and       *
 *  simulate the de-excitation process.                                         *
 *                                                                              *
 *  Author: Leo Weimer                                                          *
 *  Email: ljw00010@mix.wvu.edu                                                 *
 *  Date: August 13, 2023                                                       *
 *                                                                              *
 *  Acknowledgments:                                                            *
 *  - This code uses the CapGam library: https://www.nndc.bnl.gov/capgam/byTarget/ *
 *  - Parts of the implementation are adapted from Geant4: http://geant4.cern.ch *
 *                                                                              *
 *  To configure environment variables, please source configCASCADE.sh     *
 ********************************************************************************/

#include "G4CASCADE.hh"

using namespace std;

G4CASCADE::G4CASCADE() { }

G4CASCADE::~G4CASCADE() { }

G4ReactionProductVector* G4CASCADE::GetGammas(G4Fragment nucleus, G4bool UseRawExcitation, G4bool AlwaysEmitGamma)
{
  //Declare and initialize result vector, level data, excitation energy, and highestObtainableLevel
  G4ReactionProductVector* theResult = new G4ReactionProductVector;
  vector<vector<vector<G4double>>> levels = GetLevels(nucleus.GetZ_asInt(), nucleus.GetA_asInt());
  G4double exciteE;

  if(UseRawExcitation == 0) {
    G4double maxLevel = 0;

    //find the top energy level
    for(int c=0; c<levels.size(); c++) {
      if(levels[c][0][0] > maxLevel) {
        maxLevel = levels[c][0][0];
      }
    }

    //set excitation energy to the top level without accounting for extra energy or not enough energy
    exciteE = maxLevel;

  }else {
    exciteE = nucleus.GetExcitationEnergy();
    G4double highestObtainableLevel = 0;

    //find the highest energy level with energy less than the excitation energy
    for(int c=0; c<levels.size(); c++) {
      if(levels[c][0][0] < exciteE and levels[c][0][0] > highestObtainableLevel) {
        highestObtainableLevel = levels[c][0][0];
      }
    }

    //release excess energy as a gamma, go to highest obtainable level
    G4double excessE = exciteE - highestObtainableLevel;
    G4ReactionProduct* excessGam = new G4ReactionProduct;
    excessGam->SetDefinition( G4Gamma::Gamma() );
    excessGam->SetMomentum( excessE*GetRandomDirection() );
    theResult->push_back(excessGam);
    exciteE = highestObtainableLevel;

  }

  G4int levelIndex;
  G4double ground = 0;

  //until at ground state, randomly choose decay based on branching ratios
  while(exciteE != ground) {
    levelIndex = -1;
    for(int c=0; c<levels.size() and levelIndex == -1; c++) {
      if(levels[c][0][0] == exciteE) {
        levelIndex = c;
      }
    }
    vector<G4double> sumProbs;
    G4double prev = 0;
    for(int c=1; c<levels[levelIndex].size(); c++) {
      sumProbs.push_back(prev + levels[levelIndex][c][1]);
      prev = sumProbs[c-1];
    }
    G4double random = ((float) rand()/RAND_MAX) * sumProbs[sumProbs.size()-1];
    for(int c=0; c<sumProbs.size(); c++) {
      if(random < sumProbs[c]) {
        if(levels[levelIndex][c+1][2] == 1 * CLHEP::keV or AlwaysEmitGamma) {
          G4ReactionProduct* newGam = new G4ReactionProduct;
          newGam->SetDefinition( G4Gamma::Gamma() );
          newGam->SetMomentum( (exciteE - levels[levelIndex][c+1][0]) * GetRandomDirection() );
          theResult->push_back(newGam);
        }
        exciteE = levels[levelIndex][c+1][0];
        break;
      }
    }
  }

  return theResult;

}

//Method to check if CASCADE has data for a particular isotope
bool G4CASCADE::HasData(G4int Z, G4int A)
{
  G4String dataDir = std::getenv("CASCADE_DATA_DIR");
  G4String name = dataDir + "/" + std::to_string(Z) + "-" + std::to_string(A) + ".txt";
  ifstream f(name.c_str());
  return f.good();
}

//method to retrieve level data from CapGamData directory
vector<vector<vector<G4double>>> G4CASCADE::GetLevels(G4int Z, G4int A)
{
  G4String dataDir = std::getenv("SIMPLE_CAPGAM_DATA_DIR");
  G4String name = dataDir + "/" + std::to_string(Z) + "-" + std::to_string(A) + ".txt";
  ifstream newfile;
  newfile.open(name);
  vector<G4String> rawLines;
  G4String temp;
  while(getline(newfile, temp)) {
    rawLines.push_back(temp);
  }

  vector<vector<vector<G4String>>> levelStrings;
  G4int c=0;

  while(c<rawLines.size()) {
    vector<vector<G4String>> v2;
    if(rawLines[c].find(", ") == std::string::npos) {
      vector<G4String> v12;
      v12.push_back(rawLines[c]);
      v2.push_back(v12);
      c++;
      while(c<rawLines.size() and rawLines[c].find(", ") != std::string::npos) {
        vector<G4String> v1;
        v1.push_back(rawLines[c].substr(3, rawLines[c].find(", ")-3));
        v1.push_back(rawLines[c].substr(rawLines[c].find(", ")+2, rawLines[c].size()));
        if(rawLines[c].substr(0, 3) == " X "){
          v1.push_back("0");
        }else {
          v1.push_back("1");
        }
        v2.push_back(v1);
        c++;
      }
      levelStrings.push_back(v2);
    }else {
      c++;
    }
  }

  vector<vector<vector<G4double>>> levelDoubs;
  double val;
  for(int c1=0; c1<levelStrings.size(); c1++) {
    vector<vector<G4double>> v2;
    for(int c2=0; c2<levelStrings[c1].size(); c2++) {
      vector<G4double> v1;
      for(int c3=0; c3<levelStrings[c1][c2].size(); c3++) {
        val = stod(levelStrings[c1][c2][c3]) * CLHEP::keV;
        v1.push_back(val);
      }
      v2.push_back(v1);
    }
    levelDoubs.push_back(v2);
  }

  return levelDoubs;

}

//Method to generate a G4ThreeVector with a random direction
G4ThreeVector G4CASCADE::GetRandomDirection()
{
  CLHEP::HepRandom::setTheEngine(new CLHEP::MTwistEngine);
  CLHEP::HepRandom::setTheSeed(time(NULL));

  G4double costheta = 2.*((float) rand()/RAND_MAX)-1.;
  G4double theta = std::acos(costheta);
  G4double phi = twopi*((float) rand()/RAND_MAX);
  G4double sinth = std::sin(theta);
  G4ThreeVector direction(sinth*std::cos(phi), sinth*std::sin(phi), costheta);
  return direction;
}
