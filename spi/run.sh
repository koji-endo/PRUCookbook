#!/bin/bash

echo 46 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio46/direction
echo 1 > /sys/class/gpio/gpio46/value

sleep 1

echo stop > /sys/class/remoteproc/remoteproc1/state
echo stop > /sys/class/remoteproc/remoteproc2/state

sleep 1

echo start > /sys/class/remoteproc/remoteproc1/state
echo start > /sys/class/remoteproc/remoteproc2/state

sleep 1

./ARM_User_Space_App.out

echo 0 > /sys/class/gpio/gpio46/value
echo in > /sys/class/gpio/gpio46/direction
echo 46 > /sys/class/gpio/unexport
