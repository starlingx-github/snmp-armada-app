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

# Waiting master agent initialization
if [ "$MASTER_AGENT_HOST" == "" ]; then
    MASTER_AGENT_HOST="localhost";
fi

if [ "$MASTER_AGENT_PORT" == "" ]; then
    MASTER_AGENT_PORT=705;
fi

if [ "$MASTER_AGENT_CONNECTION_RETRIES" == "" ]; then
    MASTER_AGENT_CONNECTION_RETRIES=20;
fi

HOST=$MASTER_AGENT_HOST
PORT=$MASTER_AGENT_PORT
RETRIES=$MASTER_AGENT_CONNECTION_RETRIES

echo "Waiting master agent initialization ($HOST:$PORT) [MaxRetries:$RETRIES]"
counter=0
until </dev/tcp/$HOST/$PORT; do
    sleep 1;
    [[ counter -eq $RETRIES ]] && exit 1;
    echo "Trying again, try #$counter";
    $((counter++));
done

# Internal params
RUN_CMD="./snmpSubAgent ${OPTIONDEBUG} -f -x tcp:${HOST}:$PORT"

# Launch
$RUN_CMD
