#include <iostream>
#include <fstream>
#include <vector>
#include "G4Types.hh"
#include "G4String.hh"
#include <CLHEP/Units/SystemOfUnits.h>

using namespace std;

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

  if(dataDir.substr(dataDir.size()-1) != "/") {
    dataDir = dataDir + "/";
  }
  if(outputDir.substr(outputDir.size()-1) != "/") {
    outputDir = outputDir + "/";
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
  if(newfile.is_open() == 0) {
    cout << "error reading file" << endl;
    return 1;
  }

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
        if(rawLines[c].substr(0, 3) == " - "){
          v1.push_back("1");
        }else if(rawLines[c].substr(0, 3) == " E "){
          v1.push_back("2");
        }else if(rawLines[c].substr(0, 3) == " U "){
          v1.push_back("-1");
        }else if(rawLines[c].substr(0, 3) == " V "){
          v1.push_back("-2");
        }else if(rawLines[c].substr(0, 3) == " P "){
          v1.push_back("-3");
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

  /*
  for(int c1=0; c1<levelDoubs.size(); c1++) {
    for(int c2=0; c2<levelDoubs[c1].size(); c2++) {
      for(int c3=0; c3<levelDoubs[c1][c2].size(); c3++) {
        cout << levelDoubs[c1][c2][c3] << ", ";
      }
      cout << endl;
    }
    cout << "------" << endl;
  }
  */

  G4String writeFileName = outputDir + isotope + ".bin";
  if(verbose > 0) {
    cout << "writing to \"" << writeFileName << "\"" << endl;
  }

  std::ofstream file(writeFileName, std::ios::out | std::ios::binary);

  if (file.is_open()) {
    // Write the dimensions of the 3D vector
    size_t dim1 = levelDoubs.size();
    file.write(reinterpret_cast<char*>(&dim1), sizeof(size_t));

    for (const auto& vec2D : levelDoubs) {
      size_t dim2 = vec2D.size();
      file.write(reinterpret_cast<char*>(&dim2), sizeof(size_t));

      for (const auto& vec1D : vec2D) {
        size_t dim3 = vec1D.size();
        file.write(reinterpret_cast<char*>(&dim3), sizeof(size_t));
        file.write(reinterpret_cast<const char*>(vec1D.data()), dim3 * sizeof(double));
      }
    }

    // Close the file
    file.close();
    if(verbose > 0) {
      cout << "file written successfully" << endl;
    }
  } else {
    std::cerr << "Error: Could not open the file for writing." << std::endl;
  }

  return 0;
}
