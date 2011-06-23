#!/usr/bin/env bash
#
#PBS -r n
#PBS -l nodes=1:ppn=1:x86_64
#PBS -l walltime=5:00:00
#PBS -j oe

set -u

kkmax=20000

/usr/bin/which simulateRobot > /dev/null 2> /dev/null || exit 4

scratch="$(mktemp -t -d "run.XXXX")"
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
        rm "${definition_file}"
        rm "${performance_file}"
        continue
    fi

    let "success++"
    echo "success"
    rm "${definition_file}"
    test ${success} -ge 1000 && break
done

echo "kk=${kk} success=${success} failed=${failed}"
