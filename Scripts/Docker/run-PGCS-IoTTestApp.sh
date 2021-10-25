#!/bin/sh

# CA_PATH is the path inside container where I/O will happen. Do not change this parameter
# ALTERNATIVE is the option to start PGCS. For physical setup in laboratory, use "-pc". For emulated scenario, use "-dec"
# STACK is the technology that will carry NG messages. For physical setup in laboratory, use "Wi-Fi". For emulated scenario, use "Ethernet"
# ROLE defines if the scenario will run inside a domain or among domains (not supported yet). Do not change this parameter
# INTERFACE defines the linux interface inside the container that will be used to transmit to other containers, host OS or equipment. For physical setup in laboratory, use the Wi-Fi interface available inside a container. For emulated scenario, use "eth0".
# IDENTIFIER is the MAC address of the peer device in the case of physical mote experimentation. In case of emulated scenario, this paramenter should be "FF:FF:FF:FF:FF:FF".
# SIZE is the MTU of the packets on the interface. Do not change this parameter
#
# Usage:
#
# sh rrun-PGCS-IoTTestApp.sh ALTERNATIVE STACK INTERFACE IDENTIFIER
# 
# Example for physical experiment with NXP 1588 or ESP32 
# sh run-PGCS-IoTTestApp.sh -pc Wi-Fi wlp7s0 00:23:a7:23:06:b2
#
# Example for emulated experiment with EPGS container(s)
# sh run-PGCS-IoTTestApp.sh -dec Ethernet eth0 FF:FF:FF:FF:FF:FF

docker run -itd --privileged -e CA_PATH="/home/ng/workspace/novagenesis/IO/IoTTestApp/" -e ALTERNATIVE=$1 -e STACK=$2 -e ROLE="Intra_Domain" -e INTERFACE=$3 -e IDENTIFIER=$4 -e SIZE="1200" --name Client ng-iottestapp:latest

echo "Creating IoTTestApp Input/Output folder"

docker exec -w /home/ng/workspace/novagenesis/IO/ Client mkdir IoTTestApp

echo "Changing IoTTestApp Input/Output folder permissions"

docker exec -w /home/ng/workspace/novagenesis/IO/ Client chmod -R 777 IoTTestApp

echo "Copying IoTTestApp.ini from local Includes folder to IoTTestApp Input/Output folder"

#Copy local IoTTestApp.ini to the container to set up Client app parameters
docker cp Includes-IoTTestApp/IoTTestApp.ini Client:/home/ng/workspace/novagenesis/IO/IoTTestApp/
