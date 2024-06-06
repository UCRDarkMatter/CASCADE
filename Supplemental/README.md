# Supplemental Information for G4CASCADE

## resStats.txt

The file `resStats.txt` contains information describing the level of agreement between various G4 models for neutron capture and this model with ENSDF:

- **Ave Res**: Represents the average residual (difference) between a model and ENSDF over all energies.
- **Max Res**: Represents the energy channel which has the highest residual for that model and isotope.

These values are given for NDL (stock model), Photon Evaporation (stock model), CASCADE without unplaced gammas, and CASCADE with unplaced gammas (u). Better agreement is indicated by lower values of Ave Res and Max Res. Note that there is no residual information for 17-36, as this isotope doesn’t have valid CapGam spectrum data for comparison, though it can still be simulated by CASCADE.

## Graphs Folder

The `Graphs` folder contains relative intensity spectrum comparisons for several G4 models, including this one, vs. ENSDF. These graphs exist for each isotope simulated by CASCADE with the exception of 17-36. The x axis on the graphs (energy) is capped at 10 MeV, but many isotopes have gammas >10 MeV. These gammas are not shown on the graph. Below each graph is a residual plot which shows the residual for each model and ENSDF for each energy channel.

## Level Structure Diagram

The level structure diagram in `Supplemental` shows how the nuclear level structure is stored in a 3D vector of doubles. The large gray box represents the entire 3D vector for the level structure, the mid-sized blue boxes represent 2D vectors, the small green boxes represent 1D vectors, and the white boxes represent doubles. The yellow boxes are excluded from the 3D vector, and are only present in the binary file. The arrows represent the order in which the data is written in the binary file. All values (including emission type) are stored in units of keV. Emission types are as follows:

- `0`: No emission (not in current CASCADE version, but in older versions)
- `1`: Gamma
- `2`: Electron
- `-1`: Unplaced gamma
- `-2`: Unplaced electron
- `-3`: Transition using photon evaporation

## ASCIITranslator

`ASCIITranslator` translates data from a raw ENSDF `.txt` file into a level structure `.txt` file that is more legible and readable by `SBTranslator`.

### Running ASCIITranslator

1. Navigate to the build directory and run:
    ```sh
    cmake ..
    make
    ```
2. Set the values in `params.txt`.
3. Run the executable with the correct format:
    ```sh
    ./ASCIITranslator <filename>
    ```

### params.txt

`params.txt` must contain exactly 4 lines in the following order:
```sh
dataDir=/full/path/to/ENSDF/data
outputDir=/full/path/to/output/directory
justUseName=(0 or 1)
verbose=(integer)
```

- **justUseName**: If set to `1` (true), `ASCIITranslator` uses the given string argument as the entire path to the file to translate. If `0` (false), it uses `dataDir` and the given string to find the input file.
- **verbose**: Controls the verbosity of the output:
    - `0`: No console output unless there is a critical error.
    - `1`: Describes various aspects of its execution.
    - `2`: Describes in detail what is happening.

### Example ENSDF File

An example ENSDF file (`12_26.txt`) is provided in the `Supplemental` folder. The input file name string must have the format `Z_A.txt`, and the output file will have the format `Z-A.txt`. In both cases, A is that of the nucleus post-capture.

## SBTranslator

`SBTranslator` translates from level structure `.txt` files into `.bin` files that G4CASCADE can use.

### Running SBTranslator

Compiling and running `SBTranslator` is similar to `ASCIITranslator`, and it uses the same parameters in `params.txt`. The inputted file name string must be of the format “Z-A.txt”, and the output file will have the format “Z-A.bin”. In both cases, A is that of the nucleus post-capture.

## LevelStructureTXTs Folder

The `LevelStructureTXTs` folder contains plaintext versions of the level structure files, formatted as follows:

~~~text
 - [Level to go to energy in keV], [branching ratio]
 - [Another level to go to], [branching ratio]
 E [Another level to go to (electron (IC) emitted instead of gamma)], [branching ratio]
 U [Another level to go to (unplaced gamma transition)], [branching ratio]
 V [Another level to go to (unplaced electron transition)], [branching ratio]
 P [Another level to go to (transition utilizes Photon Evaporation)], [branching ratio]
[Another level energy]
 - [level to go to energy], [branching ratio]
~~~
Here's Carbon 13 as an example:
~~~text
	3089.45
	 - 0, 0.43
	3684.47
	 - 3089.45, 0.24
	 - 0, 32.14
	4946.31
	 - 3684.47, 32.36
	 - 3089.45, 0.16
	 - 0, 67.47
~~~
Energy values are in units of keV. Branching ratios are normalized by CASCADE during execution.
Using these `.txt` files, the database can be manually edited. This is generally not recommended, however, as CASCADE is very particular about the numbers in these files. If, for example, you write a transition to a level with energy “642.76”, but the level actually has energy “642.761”, CASCADE will output a warning and exit. CASCADE will crash if there are levels which it cannot de-excite from.
