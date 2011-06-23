#!/usr/bin/env bash

set -u

kkmax=1000

/usr/bin/which simulateRobot > /dev/null 2> /dev/null || exit 4

scratch="$(mktemp -d "run.XXXX")"
echo "scratch is ${scratch}"

failed=0
success=0
for kk in $(seq ${kkmax}); do
    echo "robot ${kk}/${kkmax} ${success} ${failed}"
    definition_file="${scratch}/definition.${kk}.pck"
    performance_file="${scratch}/performance.${kk}.pck"
    generateRobot.py "${definition_file}" > /dev/null
    simulateRobot "${definition_file}" "${performance_file}" > /dev/null 2> /dev/null

    if test $? -ne 0
    then
        echo "failed"
        let "failed++"
        rm "${definition_file}" "${performance_file}"
        continue
    fi

    let "success++"
    echo "success"

    test ${success} -ge 50 && break
done

echo "kk=${kk} success=${success} failed=${failed}"
