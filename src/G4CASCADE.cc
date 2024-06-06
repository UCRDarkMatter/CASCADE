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

G4RDShellData* shellDat;

G4CASCADE::G4CASCADE() { 
  shellDat = new G4RDShellData;
  shellDat->LoadData("/fluor/binding");
}

G4CASCADE::~G4CASCADE() { }

G4ReactionProductVector* G4CASCADE::GetGammas(G4Fragment nucleus, G4bool UseRawExcitation, G4bool doUnplaced)
{
  //Declare and initialize result vector, level data, excitation energy, and highestObtainableLevel
  G4ReactionProductVector* theResult = new G4ReactionProductVector;
  vector<vector<vector<G4double>>> levels = GetLevels(nucleus.GetZ_asInt(), nucleus.GetA_asInt());
  G4double exciteE;

  if(UseRawExcitation == 0) {
    G4double maxLevel = 0;

    //find the top energy level
    for(int c=0; c<(int)levels.size(); c++) {
      if(levels[c][0][0] > maxLevel) {
        maxLevel = levels[c][0][0];
      }
    }

    //set excitation energy to the top level without accounting for extra energy or not enough energy
    exciteE = maxLevel;

  }else {
    exciteE = nucleus.GetExcitationEnergy();

    //Emma's Q value correction
    G4double Product_GroundStateMass = nucleus.GetGroundStateMass();
    G4int isoZ = nucleus.GetZ_asInt();
    G4int isoA = nucleus.GetA_asInt();
    G4Fragment target,the_neutron;
    G4double Target_GroundStateMass = target.ComputeGroundStateMass(isoZ, isoA-1);
    G4double Neutron_GroundStateMass = the_neutron.ComputeGroundStateMass(0,1);
    G4double Q_value_estimate = Neutron_GroundStateMass+Target_GroundStateMass-Product_GroundStateMass;

    G4double maxLevel = 0;

    //find the top energy level
    for(int c=0; c<(int)levels.size(); c++) {
      if(levels[c][0][0] > maxLevel) {
        maxLevel = levels[c][0][0];
      }
    }

    G4double Q_correction = maxLevel-Q_value_estimate;
    exciteE += Q_correction;
    //end correction

    G4double highestObtainableLevel = 0;

    //find the highest energy level with energy less than the excitation energy
    for(int c=0; c<(int)levels.size(); c++) {
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
    for(int c=0; c<(int)levels.size() and levelIndex == -1; c++) {
      if(levels[c][0][0] == exciteE) {
        levelIndex = c;
      }
    }
    if(levels[levelIndex].size() == 2 and levels[levelIndex][1][2] == -3 * CLHEP::keV) {
        //Photon Evaporation implementation taken from G4particleHPCaptureFS (and modified for this context)
        G4PhotonEvaporation photonEvaporation;
        photonEvaporation.SetICM( TRUE );
        G4LorentzVector nLV = nucleus.GetMomentum();
        G4ThreeVector n3m(nLV.getX(), nLV.getY(), nLV.getZ());
        G4LorentzVector temp(n3m, exciteE + nucleus.GetGroundStateMass());
        nucleus.SetMomentum(temp);
        G4FragmentVector* products = photonEvaporation.BreakItUp(nucleus);
        G4FragmentVector::iterator it;
        for(it=products->begin(); it!=products->end(); it++)
        {
          G4ReactionProduct * theOne = new G4ReactionProduct;
          if ( (*it)->GetParticleDefinition() != 0 )
            theOne->SetDefinition( (*it)->GetParticleDefinition() );
          else
            theOne->SetDefinition( G4Gamma::Gamma() ); // this definition will be over writen

          G4IonTable* theTable = G4IonTable::GetIonTable();
          if ( (*it)->GetMomentum().mag() > 10*MeV)
            theOne->SetDefinition(theTable->GetIon(nucleus.GetZ_asInt(), nucleus.GetA_asInt(), 0) );

          if ( (*it)->GetExcitationEnergy() > 1.0e-2*eV) {
            G4double ex = (*it)->GetExcitationEnergy();
            G4ReactionProduct* aPhoton = new G4ReactionProduct;
            aPhoton->SetDefinition( G4Gamma::Gamma() );
            aPhoton->SetMomentum( (*it)->GetMomentum().vect().unit() * ex );
            theResult->push_back(aPhoton);
          }

          theOne->SetMomentum( (*it)->GetMomentum().vect() * ( (*it)->GetMomentum().t() - (*it)->GetExcitationEnergy() ) / (*it)->GetMomentum().t() ) ;
          theResult->push_back(theOne);
          delete *it;
        }
        delete products;
        exciteE = ground;
    }
    else {
      vector<G4double> sumProbs;
      G4double prev = 0;
      for(int c=1; c<(int)levels[levelIndex].size(); c++) {
        sumProbs.push_back(prev + levels[levelIndex][c][1]);
        prev = sumProbs[c-1];
      }
      G4double random = ((float) rand()/RAND_MAX) * sumProbs[sumProbs.size()-1];
      for(int c=0; c<(int)sumProbs.size(); c++) {
        if(random < sumProbs[c] and (levels[levelIndex][c+1][2] > 0 or doUnplaced)) {
          if(levels[levelIndex][c+1][2] == 1 * CLHEP::keV or (levels[levelIndex][c+1][2] == -1 * CLHEP::keV and doUnplaced)) {
            G4ReactionProduct* newGam = new G4ReactionProduct;
            newGam->SetDefinition( G4Gamma::Gamma() );
            newGam->SetMomentum( (exciteE - levels[levelIndex][c+1][0]) * GetRandomDirection() );
            theResult->push_back(newGam);
          }
  	  if(levels[levelIndex][c+1][2] == 2 * CLHEP::keV or (levels[levelIndex][c+1][2] == -2 * CLHEP::keV and doUnplaced)) {
	    G4ReactionProduct* newEl = new G4ReactionProduct;
	    newEl->SetDefinition( G4Electron::Electron() );
  	    G4RDAtomicDeexcitation* AtoDeex = new G4RDAtomicDeexcitation;
  	    vector<G4DynamicParticle*>* ADGammas;
	    G4double ICrand = ((float) rand()/RAND_MAX);

	    //Choose which shell to eject electron from based on constant percentages, do atomic deexcitation
	    if(ICrand <= 0.893){
	      ADGammas = AtoDeex->GenerateParticles(nucleus.GetZ_asInt(), 1);
	      G4double E = (exciteE - levels[levelIndex][c+1][0] - shellDat->BindingEnergy(nucleus.GetZ_asInt(), 0));
	      newEl->SetMomentum( sqrt((E * E) + (2 * 0.51*CLHEP::MeV * E)) * GetRandomDirection() );
	    }
	    else if(ICrand <= 0.982){
	      ADGammas = AtoDeex->GenerateParticles(nucleus.GetZ_asInt(), 3);
	      G4double E = (exciteE - levels[levelIndex][c+1][0] - shellDat->BindingEnergy(nucleus.GetZ_asInt(), 1));
	      newEl->SetMomentum( sqrt((E * E) + (2 * 0.51*CLHEP::MeV * E)) * GetRandomDirection() );
	    }
	    else {
	      ADGammas = AtoDeex->GenerateParticles(nucleus.GetZ_asInt(), 8);
	      G4double E = (exciteE - levels[levelIndex][c+1][0] - shellDat->BindingEnergy(nucleus.GetZ_asInt(), 4));
	      newEl->SetMomentum( sqrt((E * E) + (2 * 0.51*CLHEP::MeV * E)) * GetRandomDirection() );
	    }
	    theResult->push_back(newEl);
	    for(int c2=0; c2<(int)ADGammas->size(); c2++){
              G4ReactionProduct* ADGam = new G4ReactionProduct;
              ADGam->SetDefinition(ADGammas->at(c2)->GetDefinition());
              ADGam->SetMomentum(ADGammas->at(c2)->GetMomentum());
              theResult->push_back(ADGam);
            }
	  }
          exciteE = levels[levelIndex][c+1][0];
	  bool found = false;
	  int ind = 0;
	  while(!found and exciteE != ground){
	    if(levels[ind][0][0] == exciteE){
	      found = true;
	    }
	    ind++;
	  }
	  if(!found and exciteE != ground){
	    cout << "Error: transition " << levels[levelIndex][c+1][0] << " at level " << levels[levelIndex][0][0] << " does not go to an existing level, probably due to a typo in a manually edited level structure file." << endl;
	  }
          break;
        }
      }
    }
  }

  return theResult;

}

//Method to check if CASCADE has data for a particular isotope
bool G4CASCADE::HasData(G4int Z, G4int A)
{
  G4String dataDir = std::getenv("CAPGAM_DATA_DIR");
  G4String name = dataDir + "/" + std::to_string(Z) + "-" + std::to_string(A) + ".bin";
  ifstream f(name.c_str());
  return f.good();
}

//method to retrieve level data from CapGamData directory
vector<vector<vector<G4double>>> G4CASCADE::GetLevels(G4int Z, G4int A)
{
  vector<vector<vector<G4double>>> readVector;

    G4String dataDir = std::getenv("CAPGAM_DATA_DIR");
    std::ifstream file2(dataDir + "/" + std::to_string(Z) + "-" + std::to_string(A) + ".bin", std::ios::in | std::ios::binary);

    if (file2.is_open()) {
        size_t dim12, dim22, dim32;
        file2.read(reinterpret_cast<char*>(&dim12), sizeof(size_t));
        readVector.resize(dim12);

        for (size_t i = 0; i < dim12; ++i) {
            file2.read(reinterpret_cast<char*>(&dim22), sizeof(size_t));
            readVector[i].resize(dim22);

            for (size_t j = 0; j < dim22; ++j) {
                file2.read(reinterpret_cast<char*>(&dim32), sizeof(size_t));
                readVector[i][j].resize(dim32);
                file2.read(reinterpret_cast<char*>(readVector[i][j].data()), dim32 * sizeof(double));
            }
        }

        file2.close();
    } else {
        std::cerr << "Error: Could not open the file for reading." << std::endl;
    }

  return readVector;
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
