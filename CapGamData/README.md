# CapGamData Directory

The "CapGamData" directory contains level structure and branching ratio data for isotopes used by G4CASCADE.

## File Structure

- Each isotope has its own `.txt` file named with the format "Z-A.txt". For example, "6-13.txt" would be for Carbon 13. It's important to note that A in these files is that of the nucleus post-capture. So 6-13.txt would be the file for simulating capture on carbon 12.
- Inside each file, the data is formatted as follows:
~~~text
	[Level Energy]
	 - [Level to go to energy in keV], [branching ratio]
	 - [Another level to go to], [branching ratio]
	 X [a level to go to which can optionally not emit a gamma], [branching ratio]
	[Another level energy]
	 - [level to go to energy], [branching ratio]
~~~
- Here's Carbon 13 as an example:
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
- Each line specifies an energy level or a transition to a lower-energy level with its energy in keV and branching ratio.
- For transitions marked with an X, no gamma emission occurs (controlled by the `G4NEUTRONHP_ALWAYS_EMIT_GAMMA` environment variable).

## Usage

G4CASCADE uses the data in these files to simulate de-excitation following thermal neutron capture. To ensure accurate simulation results, verify that the data files are correctly formatted and contain the necessary information for the isotopes you intend to simulate.

## Contact

If you have questions or need assistance regarding the data files in this directory, contact Leo Weimer at ljw00010 [at] mix.wvu.edu.
