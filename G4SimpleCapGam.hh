#include "G4String.hh"
#include "G4Fragment.hh"
#include "G4Types.hh"
#include "G4ReactionProduct.hh"
#include "G4ReactionProductVector.hh"
#include "G4ThreeVector.hh"
#include "Randomize.hh"
#include "G4Gamma.hh"
#include <vector>
#include <CLHEP/Units/SystemOfUnits.h>
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include <fstream>
#include <iostream>

using namespace std;

class G4SimpleCapGam
{
  public:
    G4SimpleCapGam();
   ~G4SimpleCapGam();

    bool HasData(G4int Z, G4int A);
    vector<vector<vector<G4double>>> GetLevels(G4int Z, G4int A);
    G4ReactionProductVector* GetGammas(G4Fragment nucleus, G4bool UseRawExcitation, G4bool AlwaysEmitGamma);
    G4ThreeVector GetRandomDirection();
};
