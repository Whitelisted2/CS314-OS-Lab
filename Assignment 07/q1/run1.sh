#!/bin/bash

# want to vary:
# seed from 0 to 9
# keep num at 1000
# limit from 0 to 1024

seed=$1
num=$2
limit=$3
python3 relocation.py -s $seed -n $num -l $limit -c | grep "f="
