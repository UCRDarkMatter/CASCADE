#include <iostream>
#include <fstream>
#include <vector>
#include "G4Types.hh"
#include "G4String.hh"

using namespace std;

vector<vector<vector<G4double>>> prune(vector<vector<vector<G4double>>>, G4int);

int main(int argc, char** argv) {

  ifstream paramStream;
  paramStream.open("params.txt");
  if(paramStream.is_open() == 0) {
    cout << "couldn't find parameters" << endl;
    return 1;
  }

  G4String dataDir;
  getline(paramStream, dataDir);
  dataDir = dataDir.substr(dataDir.find("=")+1);
  G4String outputDir;
  getline(paramStream, outputDir);
  outputDir = outputDir.substr(outputDir.find("=")+1);
  G4String tempJDN;
  getline(paramStream, tempJDN);
  tempJDN = tempJDN.substr(tempJDN.find("=")+1);
  bool justUseName = (tempJDN != "0");
  G4String tempVerb;
  getline(paramStream, tempVerb);
  tempVerb = tempVerb.substr(tempVerb.find("=")+1);
  G4int verbose = stoi(tempVerb);

  bool doCC = 1;
  bool doNorm = 1;

  if(dataDir.substr(dataDir.size()-1) != "/") {
    dataDir = dataDir + "/";
  }
  if(outputDir.substr(outputDir.size()-1) != "/") {
    outputDir = outputDir + "/";
  }
  if(verbose > 1) {
    cout << "data directory set to: " << dataDir << endl;
  }
  G4String name;
  if(argc == 1) {
    cout << "please input file name: ";
    cin >> name;
  }else {
    name = argv[1];
  }
  G4String isotope = name.substr(0, name.find("."));
  G4String fileName = dataDir + name;
  if(justUseName){
    fileName = name;
    isotope = name;
    while(isotope.find("/") != std::string::npos) {
      isotope = isotope.substr(isotope.find("/")+1);
    }
    isotope = isotope.substr(0, isotope.find("."));
  }
  if(verbose > 0) {
    cout << "reading data from: " << fileName << endl;
    cout << "isotope: " << isotope << endl;
    cout << endl;
  }
  ifstream newfile;
  newfile.open(fileName);
  if(newfile.is_open() == 0){
    cout << "error reading file" << endl;
    return 1;
  }
  vector<G4String> rawLines;

  G4String temp;
  G4double NR = 1;
  G4double NT = 1;
  while(getline(newfile, temp)) {
    if(temp.size() > 0 and (temp.substr(6, 3) == " L " or temp.substr(6, 3) == " G ") and temp.substr(5, 1) == " ") {
      rawLines.push_back(temp);
    }else if (temp.size() > 0 and temp.substr(5, 3) == "  N" and doNorm) {
      if(temp.substr(9, 10).find_first_not_of(" ") != string::npos and stod(temp.substr(9, 10)) != 1) {
        if(verbose > 1) {
          cout << "attempting stod of NR" << endl;
        }
        NR = stod(temp.substr(9, 10));
        if(verbose > 1) {
          cout << "NR is now " << NR << endl;
        }
      }
      if(temp.substr(21, 8).find_first_not_of(" ") != string::npos and stod(temp.substr(21, 8)) != 1) {
        if(verbose > 1) {
          cout << "attempting stod of NT" << endl;
        }
        NT = stod(temp.substr(21, 8));
        if(verbose > 1) {
          cout << "NT is now " << NT << endl;
        }
      }
    }else if (temp.size() > 0 and temp.substr(6, 3) == " B ") {
      cout << "Beta detected, not sure what to do about that, will ignore" << endl;
    }else if (temp.size() > 0 and temp.substr(6, 3) == " E ") {
      cout << "Electron capture detected, not sure what to do about that, will ignore" << endl;
    }else if (temp.size() > 0 and temp.substr(6, 3) == " A ") {
      cout << "Alpha detected, not sure what to do about that, will ignore" << endl;
    }else if (temp.size() > 0 and temp.substr(7, 1) == "D") {
      cout << "Delayed emission detected, not sure what to do about that, will ignore" << endl;
    }
  }

  if(verbose > 1){
    cout << "lines in consideration:" << endl;
    for(int i=0; i<rawLines.size(); i++) {
      cout << rawLines[i] << endl;
    }
  }

  vector<vector<vector<G4double>>> levels;
  G4double CC;
  G4int c=0;

  //cout << "pre-while" << endl;
  while(c<rawLines.size()) {
    vector<vector<G4double>> v2;
    //cout << "line: " << rawLines[c] << endl;
    if(rawLines[c].substr(6, 3) == " L ") {
      vector<G4double> v12;
      v12.push_back(stod(rawLines[c].substr(9, 10)));
      v2.push_back(v12);
      c++;
      //if(c<rawLines.size()){
      //  cout << "gamma: " << rawLines[c] << endl;
      //}
      while(c<rawLines.size() and rawLines[c].substr(6, 3) == " G ") {
        CC = 0;
        vector<G4double> v1;
	vector<G4double> v1e;
        v1.push_back(stod(rawLines[c].substr(9, 10)));
        if(rawLines[c].substr(55, 7).find_first_not_of(" ") != string::npos and doCC) {
          //cout << "attempting stod of CC" << endl;
          CC = stod(rawLines[c].substr(55, 7));
        }
	if(CC > 0){
	  v1e.push_back(stod(rawLines[c].substr(9, 10)));
	}
        if(rawLines[c].substr(64, 10).find_first_not_of(" ") != string::npos) {
          if(rawLines[c].substr(21, 8).find_first_not_of(" ") == string::npos) {
            if(verbose > 0) {
              cout << "the " << c << "th line of rawLines had a TI, but no RI" << endl;
            }
            //cout << "attempting stod of TI" << endl;
            v1.push_back((stod(rawLines[c].substr(64, 10)) * NT) / (CC+1));
	    v1.push_back(1);
	    if(CC > 0) {
	      v1e.push_back((stod(rawLines[c].substr(64, 10)) * NT) - ((stod(rawLines[c].substr(64, 10)) * NT) / (CC+1)));
	      v1e.push_back(2);
	    }
          }else {
            v1.push_back(stod(rawLines[c].substr(21, 8)) * NR);
	    v1.push_back(1);
	    if(CC > 0) {
	      v1e.push_back(stod(rawLines[c].substr(21, 8)) * NR * CC);
	      v1e.push_back(2);
	    }
          }
        }else if(rawLines[c].substr(21, 8).find_first_not_of(" ") != string::npos){
          //cout << "attempting stod of RI" << endl;
          v1.push_back(stod(rawLines[c].substr(21, 8)) * NR);
          v1.push_back(1);
          if(CC > 0) {
            v1e.push_back(stod(rawLines[c].substr(21, 8)) * NR * CC);
            v1e.push_back(2);
          }
        }else {
          if(verbose > 0) {
            cout << "the " << c << "th line of rawLines had neither an RI nor a TI, so it's BR was set to 0, marking it for deletion" << endl;
          }
          v1.push_back(0);
        }
        v2.push_back(v1);
	if(CC > 0) {
	  v2.push_back(v1e);
	}
        c++;
      }
      levels.push_back(v2);
    }else if(rawLines[c].substr(6, 3) == " G ") {
      rawLines.push_back(rawLines[c]);
      c++;
    }else {
      c++;
    }
  }

  /*
  for(int i1=0; i1<levels.size(); i1++) {
    for(int i2=0; i2<levels[i1].size(); i2++) {
      for(int i3=0; i3<levels[i1][i2].size(); i3++){
        cout << levels[i1][i2][i3] << endl;
      }
    }
    cout << "--------" << endl;
  }
  */

  //cout << "post-while" << endl;
  G4double nearestDiff;
  G4int nearestInd;
  G4double Tolerance = 10;
  for(int c1=0; c1<levels.size(); c1++) {
    for(int c2=1; c2<levels[c1].size(); c2++) {
      //cout << "for cond done" << endl;
      nearestInd = -1;
      nearestDiff = 999999999999;
      for(int l=0; l<levels.size(); l++){
        if(fabs((levels[c1][0][0] - levels[c1][c2][0]) - levels[l][0][0]) < nearestDiff) {
          nearestInd = l;
          nearestDiff = fabs((levels[c1][0][0] - levels[c1][c2][0]) - levels[l][0][0]);
        }
      }
      //cout << "neareset level found" << endl;
      if(nearestDiff < Tolerance) {
        if(verbose > 1) {
          cout << "gamma " << levels[c1][c2][0] << " at level " << levels[c1][0][0] << " was corresponded to " << levels[nearestInd][0][0] << endl;
        }
        levels[c1][c2][0] = levels[nearestInd][0][0];
      }else if(levels[c1][c2][0] > 1000) {
        if(verbose > 0) {
          cout << "couldn't find level for gamma " << levels[c1][c2][0] << " at level " << levels[c1][0][0] << " within tolerance " << Tolerance << ", treating as unplaced gamma" << endl;
        }
	levels[c1][c2][2] = levels[c1][c2][2] * -1;
        G4double newLevel = levels[c1][0][0] - levels[c1][c2][0];
	//cout << newLevel << endl;
	int l=0;
	while(levels[l][0][0] < newLevel) {
	  l++;
	}
	vector<G4double> v1;
	v1.push_back(newLevel);
	vector<vector<G4double>> v2;
	v2.push_back(v1);
	levels.insert(levels.begin() + l, v2);
	if(l <= c1) {
	  c1++;
	}
	levels[c1][c2][0] = newLevel;
      }else {
        if(verbose > 0) {
          cout << "couldn't find level for gamma " << levels[c1][c2][0] << " at level " << levels[c1][0][0] << " within tolerance " << Tolerance << ", removing gamma (E < 1 MeV)" << endl;
        }
        levels[c1][c2][0] = 0;
        levels[c1][c2][1] = 0;
        levels[c1][c2][2] = 0;
      }
    }
  }

  /*
  cout << "levels (pre-prune): " << endl;
  for(int i1=0; i1<levels.size(); i1++) {
    for(int i2=0; i2<levels[i1].size(); i2++) {
      for(int i3=0; i3<levels[i1][i2].size(); i3++){
        cout << levels[i1][i2][i3] << ", ";
      }
      cout << endl;
    }
    cout << "--------" << endl;
  }
  */

  vector<vector<vector<G4double>>> levelsPruned(levels);

  levelsPruned = prune(levelsPruned, 0);

  for(int c1=0; c1<levelsPruned.size(); c1++){
    if(levelsPruned[c1].size() == 0) {
      levelsPruned.erase(levelsPruned.begin() + c1);
      c1--;
    }
  }

  G4double maxLevel = 0;
  for(int c1=0; c1<levels.size(); c1++) {
    if(levels[c1][0][0] > maxLevel) {
      maxLevel = levels[c1][0][0];
    }
  }
  bool hasMax = false;
  for(int c1=0; c1<levelsPruned.size(); c1++) {
    if(levelsPruned[c1][0][0] == maxLevel) {
      hasMax = true;
    }
  }
  if(hasMax == false) {
    if(verbose > 0) {
      cout << "The max level of " << isotope << " was pruned, likely due to bad ENSDF data. System will not output, and will now exit." << endl;
    }
    return 0;
  }

  /*
  cout << "levelsPruned: " << endl;
  for(int i1=0; i1<levelsPruned.size(); i1++) {
    for(int i2=0; i2<levelsPruned[i1].size(); i2++) {
      for(int i3=0; i3<levelsPruned[i1][i2].size(); i3++){
        cout << levelsPruned[i1][i2][i3] << ", ";
      }
      cout << endl;
    }
    cout << "--------" << endl;
  }
  */

  for(int c1=0; c1<levels.size(); c1++) {
    if(levels[c1].size() <= 1 and levels[c1][0][0] > 0) {
      vector<G4double> v1;
      v1.push_back(0);
      v1.push_back(1);
      v1.push_back(-3);
      levels[c1].push_back(v1);
    }
  }

  for(int c1=0; c1<levels.size(); c1++) {
    for(int c2=0; c2<levels[c1].size(); c2++) {
      bool pruned = true;
      for(int i1=0; i1<levelsPruned.size() and pruned; i1++) {
        for(int i2=0; i2<levelsPruned[i1].size() and pruned; i2++) {
          if(levels[c1][c2][0] == levelsPruned[i1][i2][0] and levels[c1][0][0] == levelsPruned[i1][0][0]) {
            pruned = false;
          }
        }
      }
      if(pruned and levels[c1][c2][2] > 0) {
        levels[c1][c2][2] *= -1;
      }
    }
  }

  levels = prune(levels, verbose);

  for(int c1=0; c1<levels.size(); c1++){
    if(levels[c1].size() == 0) {
      levels.erase(levels.begin() + c1);
      c1--;
    }
  }

  /*
  cout << "levels (post-prune): " << endl;
  for(int i1=0; i1<levels.size(); i1++) {
    for(int i2=0; i2<levels[i1].size(); i2++) {
      for(int i3=0; i3<levels[i1][i2].size(); i3++){
        cout << levels[i1][i2][i3] << ", ";
      }
      cout << endl;
    }
    cout << "--------" << endl;
  }
  */

  if(levels.size() < 2) {
    cout << "Level structure for " << isotope << " was pruned completely, leaving an empty file. System will not output, and will now exit." << endl;
    return 0;
  }

  G4String writeFileName = outputDir + isotope.replace(isotope.find("_"), 1, "-") + ".txt";
  if(verbose > 0) {
    cout << "writing to \"" << writeFileName << "\"" << endl;
  }

  ofstream fw(writeFileName, std::ofstream::out);

  if(fw.is_open()){
    for(int n1=0; n1<levels.size(); n1++) {
      if(levels[n1].size() > 1) {
        for(int n2=0; n2<levels[n1].size(); n2++) {
          if(levels[n1][n2].size() == 1){
            if(levels[n1][n2][0] != 0){
              fw << levels[n1][n2][0] << endl;
            }
          }else if(levels[n1][n2].size() == 3){
            bool contains = 0;
            for(int k=0; k<levels.size(); k++) {
              if(levels[k][0][0] == levels[n1][n2][0]) {
                contains = 1;
              }
            }
            if(contains) {
              if(levels[n1][n2][2] == 1) {
                fw << " - " << levels[n1][n2][0] << ", " << levels[n1][n2][1] << endl;
              }else if(levels[n1][n2][2] == 2) {
                fw << " E " << levels[n1][n2][0] << ", " << levels[n1][n2][1] << endl;
	      }else if(levels[n1][n2][2] == -1) {
                fw << " U " << levels[n1][n2][0] << ", " << levels[n1][n2][1] << endl;
	      }else if(levels[n1][n2][2] == -2) {
	        fw << " V " << levels[n1][n2][0] << ", " << levels[n1][n2][1] << endl;
	      }else if(levels[n1][n2][2] == -3) {
	        fw << " P " << levels[n1][n2][0] << ", " << levels[n1][n2][1] << endl;
              }else {
		cout << "I (the translator) don't know what a transition of type " << levels[n1][n2][2] << " is" << endl;
	      }
            }else {
              if(verbose > 0) {
                cout << "ignored gamma to " << levels[n1][n2][0] << " because there is no such level (it was probably deleted as a dead end)" << endl;
              }
            }
          }else {
            cout << "error with data, too many entries in 1D array" << endl;
          }
        }
      }else {
        if(verbose > 0 and levels[n1][0][0] != 0) {
          cout << "ignored level " << levels[n1][0][0] << " because it was a dead end" << endl;
        }
        levels[n1][0][0] = 0;
      }
    }
    if(verbose > 0) {
      cout << "file written successfully" << endl;
    }
  }else {
    cout << "error opening file to write to";
  }

}

vector<vector<vector<G4double>>> prune(vector<vector<vector<G4double>>> levels, G4int verbose) {
  bool done = 1;
  for(int c1=0; c1<levels.size(); c1++) {
    //cout << "levels[c1][0][0]: " << levels[c1][0][0] << ", size: " << levels[c1].size() << endl;
    if(levels[c1].size() == 1 and levels[c1][0][0] > 0){
      if(verbose > 0) {cout << "removing level " << levels[c1][0][0] << endl;}
      G4double temp = levels[c1][0][0];
      levels.erase(levels.begin()+c1);
      c1--;
      done = 0;
    }
  }
  for(int c1=0; c1<levels.size(); c1++) {
    for(int c2=1; c2<levels[c1].size(); c2++) {
      bool contains = 0;
      //cout << "considering gamma " << levels[c1][c2][0] << " on level " << levels[c1][0][0] << endl;
      for(int i1=0; i1<levels.size(); i1++) {
        if(levels[i1].size() > 0 and levels[i1][0][0] == levels[c1][c2][0]) {
          contains = 1;
          //cout << "level was found for gamma" << endl;
          break;
        }
      }
      if(contains == 0) {
        if(verbose > 0) {cout << "removing gamma " << levels[c1][c2][0] << " from level " << levels[c1][0][0] << " because that gamma goes to a level that was removed." << endl;}
        levels[c1].erase(levels[c1].begin()+c2);
        c2--;
        done = 0;
      }
      else if(levels[c1][c2][1] == 0) {
        if(verbose > 0) {cout << "removing gamma " << levels[c1][c2][0] << " from level " << levels[c1][0][0] << " because that gamma had a BR of 0." << endl;}
        levels[c1].erase(levels[c1].begin()+c2);
        c2--;
        done = 0;
      }
    }
  }
  for(int c1=0; c1<levels.size() and done == 1; c1++){
    if(levels[c1].size() == 1 and levels[c1][0][0] > 0) {
      done = 0;
    }
  }
  if(done){
    return levels;
  }else{
    levels = prune(levels, verbose);
    return levels;
  }
}
