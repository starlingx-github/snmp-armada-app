#!/bin/bash
set -u

# Internal params
RUN_CMD="./snmpSubAgent -f -x tcp:localhost:705"

# Launch
$RUN_CMD

