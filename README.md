# G4SimpleCapGam

G4SimpleCapGam is an extension for Geant4 that simulates de-excitation following thermal neutron capture. It models the process of randomly moving down the level structure of isotopes according to branching ratios, emitting gamma rays based on energy differences between levels. The level structure and branching ratios for each isotope are stored in the "CapGamData" database.

## Installation

This version of G4SimpleCapGam is designed for Geant4-10.7.4. It may work with other versions, but this is not guaranteed.

###Using SimpleCapGam with a Geant4 example

Copy the example directory (for example, examples/extended/hadronic/Hadr03) to a different directory where it can be modified, then place G4SimpleCapGam.cc, G4ParticleHPCaptureFS.cc, G4ParticleHPMessenger.cc, and G4ParticleHPManager.cc into the /src directory. Place G4SimpleCapGam.hh into the /include directory. compile the example by going to the main example directory (ex: Hadr03) and running the command "make". The example should automatically compile the new files and use them instead of the default Geant files. It's important to note that this example needs to use the high-precision neutron capture model (HP) in order to utilize G4SimpleCapGam.

###Using SimpleCapGam with other Geant4 programs

place G4SimpleCapGam.cc, G4ParticleHPCaptureFS.cc, G4ParticleHPMessenger.cc, and G4ParticleHPManager.cc into the /src directory of your project, and make sure this directory is properly included in CMakeLists.txt. Place G4SimpleCapGam.hh into the /include directory, and make sure this directory is included in CMakeLists.txt. Make sure your project is using a physics list which includes the high precision neutron capture model. build and compile your project with G4SimpleCapGam using CMake.

## Usage

before using G4SimpleCapGam, source the file "configSimpleCapGam.sh". Make sure that the environment variable SIMPLE_CAPGAM_DATA_DIR is set to the correct location of CapGamData.

In your macro file, or using your UI, the command /process/had/particle_hp/use_simple_capgam true will tell Geant to use SimpleCapGam. If set to false, the default Geant methods will be used.

The command /process/had/particle_hp/use_raw_excitation [true/false] sets the value of G4NEUTRONHP_USE_RAW_EXCITATION (see Environment Variables section).

The command /process/had/particle_hp/always_emit_gamma [true/false] sets the value of G4NEUTRONHP_ALWAYS_EMIT_GAMMA (see Environment Variables section).

### CapGamData Directory

The "CapGamData" directory contains files that store the level structure and branching ratios for each isotope.

Refer to the separate README in the "CapGamData" directory for more details on the file structure.

## Environment Variables

G4SimpleCapGam uses several environment variables to control its behavior:

- **G4NEUTRONHP_USE_SIMPLE_CAPGAM**: Set to 1 (true) to use G4SimpleCapGam for generating gammas.
- **G4NEUTRONHP_USE_RAW_EXCITATION**: Set to 1 (true) to use the excitation energy of the nucleus post-capture as in G4ParticleHPCaptureFS. Set to 0 (false) for a fixed excitation energy.
- **G4NEUTRONHP_ALWAYS_EMIT_GAMMA**: Set to 1 (true) to emit a gamma for every transition, including those marked with an X. Set to 0 (false) for default behavior.

### Other Environment Variables

- **SIMPLE_CAPGAM_DATA_DIR**: Set the directory of the CapGamData database (default: ~/SimpleCapGam/CapGamData).

## Contact

For more information about SimpleCapGam, contact Leo Weimer at ljw00010@mix.wvu.edu

## Acknowledgments

Thank you to Dr. Shawn Westerdale and Ryan Krismer for helping me develop and test my code.

This code utilizes the ENSDF database and Geant4. It is written in C++.
