#!/usr/bin/env bash

set -u

rundir="${1:?"provide rundir"}"
find "${rundir}" -type f -name '*.pck' | wc -l
