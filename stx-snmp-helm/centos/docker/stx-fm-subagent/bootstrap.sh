#!/bin/bash
set -u

# Option handling in sh
if [ "$OPTIONDEBUG" != "" ]; then
    case "$OPTIONDEBUG" in
        -DALL) echo "-DALL - debug option enable"
            ;;
        -*) echo "Unrecognized option $OPTIONDEBUG"
            OPTIONDEBUG=""
            ;;
        *) echo "Bad argument $OPTIONDEBUG"
            OPTIONDEBUG=""
            ;;
    esac
fi

# Internal params
RUN_CMD="./snmpSubAgent ${OPTIONDEBUG} -f -x tcp:localhost:705"

# Launch
$RUN_CMD
