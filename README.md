# G4CASCADE

G4CASCADE is an extension for Geant4 that simulates de-excitation following thermal neutron capture. It models the process of randomly moving down the level structure of isotopes according to branching ratios, emitting gamma rays based on energy differences between levels. The level structure and branching ratios for each isotope are stored in the "CapGamData" database.

## Compatibility

This version of G4CASCADE is designed for Geant4-10.7.4.

## Getting Started

1. **Source the Configuration File**: Before using G4CASCADE, source the file `configCASCADE.sh`.
2. **Set Environment Variables**: Ensure the environment variable `CAPGAM_DATA_DIR` is set to the correct location of `CapGamData`.

## Commands

- `/process/had/particle_hp/use_CASCADE [true/false]`: Tells Geant to use CASCADE. If set to false, the default Geant methods will be used.
- `/process/had/particle_hp/use_raw_excitation [true/false]`: Sets the value of `G4NEUTRONHP_USE_RAW_EXCITATION`. For best relative intensity agreement, set to false.
- `/process/had/particle_hp/do_unplaced [true/false]`: Sets the value of `G4NEUTRONHP_DO_UNPLACED`. For best relative intensity agreement, set to false.

## Running G4CASCADE with an Example

1. Run the example as you normally would in its directory.
2. Copy the example directory (e.g., `examples/extended/hadronic/Hadr03`) to a different directory where it can be modified.
3. Place all the files from G4CASCADE’s `src` folder into the `/src` directory of the example.
4. Place all the files from G4CASCADE’s `include` folder into the `/include` directory of the example.
5. Compile the example by navigating to the main example directory (e.g., `Hadr03`) and running the command `make`. The example should automatically compile the new files and use them instead of the default Geant files.
6. Ensure the example uses the high-precision neutron capture model (HP). Otherwise G4CASCADE will not be used.

## Using G4CASCADE with Other Geant4 Programs

1. Place all the files from G4CASCADE’s `src` folder into the `/src` directory of your project and ensure this directory is properly included in `CMakeLists.txt`.
2. Place all the files from G4CASCADE’s `include` folder into the `/include` directory and ensure this directory is included in `CMakeLists.txt`.
3. Make sure your project uses a physics list that includes the high-precision neutron capture model.
4. Build and compile your project with G4CASCADE using CMake.

## Important Notes

- Check G4CASCADE's agreement/accuracy for the isotope(s) you're simulating.

## CapGamData Directory

Each file in CapGamData contains serialized binary data that describes a 3D vector of doubles which encodes the nuclear level structure of the post-capture isotope. Each 2D vector in the 3D vector represents a level, with the first element of the first array being the energy of that level. Each 1D vector within each 2D vector represents a transition on that level, with the first element being the level to go to, the second element being the branching ratio, and the third element being the type of transition:

 - 0 = no emission (not in current CASCADE version, but in older versions)
 - 1 = gamma
 - 2 = electron
 - -1 = unplaced gamma
 - -2 = unplaced electron
 - -3 = transition using photon evaporation

The elements of each array are preceded by the size of the array in the .bin file. There is a diagram in the “Supplemental” folder to help visualize the way the level structure is encoded into the 3D vector and binary file. More information on this diagram can be found in the README file in the Supplemental folder.

## Environment Variables

- `G4NEUTRONHP_USE_CASCADE`: Set to `1` (true) to use G4CASCADE.
- `G4NEUTRONHP_USE_RAW_EXCITATION`: Set to `1` (true) to use the excitation energy of the nucleus post-capture as in G4ParticleHPCaptureFS plus a correction factor. Set to `0` (false) for a fixed excitation energy.
- `G4NEUTRONHP_DO_UNPLACED`: Set to `1` (true) to simulate unplaced gammas in the ENSDF nuclear level structure. This will require the use of Photon Evaporation to de-excite further. Set to `0` (false) to ignore unplaced gammas.
- `CAPGAM_DATA_DIR`: Set the directory of the CapGamData database.

## Preventing Simulation of Certain Isotopes

To prevent CASCADE from simulating a certain isotope, remove that isotope’s file from `CapGamData`. Geant4 will then use its default methods to simulate neutron capture for this isotope.

## Documentation and Contact

For information about how to manually modify the database and for documentation of G4CASCADE’s effectiveness for all available isotopes (except 17-36), see the `Supplemental` folder and its README file.

For more information about G4CASCADE, contact Leo Weimer at ljw00010 [at] mix.wvu.edu.

## Acknowledgements

Thank you to Shawn Westerdale, Michela Lai, Emma Ellingwood, Luis Sarmiento Pico, Ryan Krismer, and many others from DEAP-3600, Darkside 20k, and CERN for helping develop and test this code.
