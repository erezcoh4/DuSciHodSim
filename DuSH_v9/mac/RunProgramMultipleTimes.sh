#!/bin/bash

# run DuSi program multiple times (many proton generations)
# usage:
# ./mac/RunProgramMultipleTimes.sh debug
# ./mac/RunProgramMultipleTimes.sh EJ204_WG_side27mm_length200_margin3cm
#
#

declare -i Nruns=200
simName=$1;
workdir='/Users/erezcohen/Desktop/Projects/DubnaSciHod/Software/DuSciHodSim/DuSH_v9'

#
echo
echo
echo 'running ' $simName $Nruns 'times'
csvPath='/Users/erezcohen/Desktop/data/DuSiHodSim/output/csv_files'
outPath=$csvPath'/'$simName
echo 'creating'$outPath
echo
mkdir $outPath

for repIdx in $(seq 1 1 $Nruns)
do

    cd $workdir
    echo 'in' $workdir


    # run simulation
    ./RunProgram $simName

    # move output file and change simulation name
    cd $csvPath
    echo 'in'$csvPath', moving version_9.0_'$simName'_photons.csv to' $outPath'/rep_'$repIdx'.csv'
    
    mv 'version_9.0_'$simName'_photons.csv' $outPath'/rep_'$repIdx'.csv'
    
    echo
    echo 'done repetition' $repIdx
    echo
    
done

echo 'done.'
