#!/usr/bin/env bash

set -u

kkmax=100
failed=0

rm -f "definition.*.pck"
rm -f "performance.*.pck"

for kk in $(seq ${kkmax}); do
    echo "robot ${kk}/${kkmax} ${failed}"
    definition_file="definition.${kk}.pck"
    performance_file="performance.${kk}.pck"
    generateRobot.py "${definition_file}"
    simulateRobot "${definition_file}" "${performance_file}" || let "failed++" && continue
done
