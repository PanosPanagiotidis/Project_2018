#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

WORKLOAD_DIR=${1-$DIR/workloads/small}
WORKLOAD_DIR=$(echo $WORKLOAD_DIR | sed 's:/*$::')

cd $WORKLOAD_DIR

WORKLOAD=$(basename "$PWD")
echo $WORKLOAD
echo execute $WORKLOAD ...
$DIR/harness *.init *.work *.result /run.sh
