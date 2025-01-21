#!/bin/bash
DEST_IP="$1"
DEBUG_PORT="$2"
SRC_DIR="$3"
BINARY="$4"
DEST_DIR="$5"
USER="$6"
PASS="$7"
CFG_DIR="$8"

# kill gdbserver on target and delete old binary
sshpass -p ${PASS} ssh ${USER}@${DEST_IP} "sh -c 'pkill -f gdbserver; rm -rf ${DEST_DIR}/${BINARY} || exit 0'"

# send binary to target
sshpass -p ${PASS} scp ${SRC_DIR}${BINARY} ${USER}@${DEST_IP}:${DEST_DIR}/${BINARY}

# send config to target
sshpass -p ${PASS} scp -r ${CFG_DIR} ${USER}@${DEST_IP}:${DEST_DIR}

# start gdbserver on target
# sshpass -p ${PASS} ssh -t ${USER}@${DEST_IP} "sh -c 'cd ${DEST_DIR}; gdbserver localhost:${DEBUG_PORT} ${BINARY}'"
# If you need to run the binary with root privileges, instead of above line you can uncomment the alternative command:
sshpass -p ${PASS} ssh -t ${USER}@${DEST_IP} "cd "$DEST_DIR"; echo "$PASS" | sudo -S gdbserver localhost:$DEBUG_PORT $BINARY"