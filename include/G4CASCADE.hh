#include "G4String.hh"
#include "G4Fragment.hh"
#include "G4Types.hh"
#include "G4ReactionProduct.hh"
#include "G4ReactionProductVector.hh"
#include "G4ThreeVector.hh"
#include "Randomize.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4RDAtomicDeexcitation.hh"
#include <vector>
#include <CLHEP/Units/SystemOfUnits.h>
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include <fstream>
#include <iostream>
#include <math.h>

#include "G4RDAtomicDeexcitation.hh"
#include "G4RDShellData.hh"

#include "G4PhotonEvaporation.hh"
#include "G4IonTable.hh"

using namespace std;

class G4CASCADE
{
  public:
    G4CASCADE();
   ~G4CASCADE();

    bool HasData(G4int Z, G4int A);
    vector<vector<vector<G4double>>> GetLevels(G4int Z, G4int A);
    G4ReactionProductVector* GetGammas(G4Fragment nucleus, G4bool UseRawExcitation, G4bool doUnplaced);
    G4ThreeVector GetRandomDirection();
};
