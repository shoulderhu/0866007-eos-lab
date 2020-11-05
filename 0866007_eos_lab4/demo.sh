#!/bin/sh

set -x
# set -e

rmmod -f mydev
insmod mydev.ko

./writer Kevin &
./reader 192.168.0.200 4444 /dev/mydev
