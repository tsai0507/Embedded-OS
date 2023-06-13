#!/bin/sh

set -x
# set -e

rmmod -f mydev
insmod mydev.ko

./writer Daniel &
./reader 192.168.89.55 8888 /dev/mydev
