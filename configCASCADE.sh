#!/bin/bash

#configuration file for G4CASCADE, sets to optimal settings for relative intensity and correlation agreement

export G4NEUTRONHP_USE_CASCADE=0
export G4NEUTRONHP_USE_RAW_EXCITATION=0
export G4NEUTRONHP_DO_UNPLACED=0

#Change this to your data directory:
export CAPGAM_DATA_DIR=~/CASCADE/CapGamData
