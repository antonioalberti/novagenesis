#!/bin/sh

# ALTERNATIVE is the option to start PGCS. For physical setup in laboratory, use "-pc". For emulated scenario, use "-dec"
# STACK is the technology that will carry NG messages. For physical setup in laboratory, use "Wi-Fi". For emulated scenario, use "Ethernet"
# INTERFACE defines the linux interface inside the container that will be used to transmit to other containers, host OS or equipment. For physical setup in laboratory, use the Wi-Fi interface available inside a container. For emulated scenario, use "eth0".
# IDENTIFIER is the MAC address of the peer device in the case of physical mote experimentation. In case of emulated scenario, this paramenter should be "FF:FF:FF:FF:FF:FF".
#
# Usage:
#
# sh run-iottest-application.sh ALTERNATIVE STACK INTERFACE IDENTIFIER
# 
# Example for physical experiment with NXP 1588 or ESP32 
# sh run-iottest-application.sh -pc Wi-Fi wlp7s0 00:23:a7:23:06:b2
#
# Example for emulated experiment with EPGS container(s)
# sh run-iottest-application.sh -dec Ethernet eth0 FF:FF:FF:FF:FF:FF

docker container prune -f

echo "Creating three processes:"
echo "An IoT testing application"
echo "A PGCS with enabled Core block for contract establishment"
echo "A NRNCS for temporary IoT data caching in the network" 

sh run-PGCS-NRNCS-IoTTestApp.sh $1 $2 $3 $4

if [ "$1" = "-dec" ];
then
  echo "Creating EPGS" 

  sh run-EPGS.sh
fi


