#!/bin/sh

GPIO_PINS_PATH="/sys/class/gpio/"
GPIO_BASE_OFFSET=458

#
# 475 485 480 output
# 471 484 477 input

# Our raspberry is wired as follows:
# White sensor:
#   TRIG - GPIO22 (PIN15)
#   ECHO - GPIO13 (PIN33)
WHITE_TRIG_PIN=$((${GPIO_BASE_OFFSET} + 22))
WHITE_ECHO_PIN=$((${GPIO_BASE_OFFSET} + 13))

TRIG_PIN=${WHITE_TRIG_PIN}
ECHO_PIN=${WHITE_ECHO_PIN}

echo ${TRIG_PIN} > ${GPIO_PINS_PATH}/export
echo ${ECHO_PIN} > ${GPIO_PINS_PATH}/export

echo out > ${GPIO_PINS_PATH}/gpio${TRIG_PIN}/direction
echo in >  ${GPIO_PINS_PATH}/gpio${ECHO_PIN}/direction

while [ 1 ]; do
        echo 1 > /sys/class/gpio/gpio${TRIG_PIN}/value
        sleep 0.001
        cat /sys/class/gpio/gpio${ECHO_PIN}/value
        echo 0 > /sys/class/gpio/gpio${TRIG_PIN}/value
        sleep 0.001
        cat /sys/class/gpio/gpio${ECHO_PIN}/value
done
