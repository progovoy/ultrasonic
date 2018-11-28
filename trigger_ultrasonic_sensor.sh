#!/bin/sh
D="/sys/class/gpio/"
base=458

#
# 475 485 480 output
# 471 484 477 input

echo 475 > ${D}/export
echo 484 > ${D}/export

echo 477 > ${D}/export
echo 485 > ${D}/export

echo 480 > ${D}/export
echo 471 > ${D}/export

echo out > ${D}/gpio475/direction
echo out > ${D}/gpio485/direction
echo out > ${D}/gpio480/direction
echo in >  ${D}/gpio471/direction
echo in >  ${D}/gpio477/direction
echo in >  ${D}/gpio484/direction

# Green sonar
OUT="475"
IN="484"

# blue sonar
BLUE_IN="477"
BLUE_OUT="485"

# white sonar
WHITE_IN="471"
WHITE_OUT="480"
#
#
#insmod ./ultrasonicnet.ko cpu=1 sleep_us=10
#insmod ./ultrasonic.ko cpu=1 sleep_us=10

while [ 1 ]; do
        echo 1 > /sys/class/gpio/gpio${WHITE_OUT}/value
        sleep 0.0001
        echo 0 > /sys/class/gpio/gpio${WHITE_OUT}/value
        sleep 0.0001
        cat /sys/class/gpio/gpio${WHITE_IN}/value
done