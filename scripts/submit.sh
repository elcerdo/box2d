#!/bin/bash

set -u

maindir="$(pwd)"
outputdir="$(mktemp --tmpdir=${maindir} -d gen.XXXX)"
runid="${outputdir##*.}"
kkmax=100

echo "runid is ${runid}"

for kk in $(seq ${kkmax}); do
    qsub -N "robot.${runid}" -o "${outputdir}" -v "TMPDIR=${outputdir}" generation.sh  || exit 2
done
