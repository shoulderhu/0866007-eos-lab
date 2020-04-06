#!/bin/sh

set -x
set -e

make ARCH=arm clean
make ARCH=arm

rmmod -f mydev
insmod mydev.ko

./writer 9 &
./reader 192.168.0.200 4444 /dev/mydev
