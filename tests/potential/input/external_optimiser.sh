#!/bin/bash

settings=$1
structure=$2

declare -r log="external_optimiser.log"

echo "settings file: $settings" > $log
echo "structure file: $structure" >> $log

echo "==settings contents==" >> $log
cat $settings >> $log

echo -e "\n==structure contents==" >> $log
cat $structure >> $log

echo -e "\n\n# OPTIMISATION OUTCOME " >> $settings
echo "successful: true" >> $settings
echo "finalEnthalpy: 2" >> $settings
echo "finalInternalEnergy: 1" >> $settings
echo "finalPressure: 0" >> $settings
echo "finalIters: 1000" >> $settings

exit 0
