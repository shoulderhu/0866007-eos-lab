#!/bin/sh

set -x
# set -e

rmmod -f mydev
insmod mydev.ko

./writer 65 &
./reader 192.168.0.200 4444 /dev/mydev
