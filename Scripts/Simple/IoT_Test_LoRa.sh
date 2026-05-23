#!/bin/bash

BASE=`cd ../..; pwd`;

######################## NOTES ###################################

#this script starts NG with the main componets
#In the local scnario there are two elements 
#	ESP32-01 MAC: e0:e2:e6:00:71:0c
#	ESP32-02 MAC: 24:6f:28:22:2c:88

# Topology
# NG (install in a PC) <=> Wi-Fi Ap <=> GW (LoRa <=> WiFi) <=> EPGS (NG embeded)
#  ____________		     ____________		_______________             ___________
#  |           |		|           |		|             |            |          |
#  |   NG      |	    |    WIFI	|       |  LORA-WIFI  |	LoRA	   |   EPGS_  |
#  |           |		|    AP     |       |   GATEWAY   |            | LORA_NODE|
#  |___________|		|___________|		|_____________|            |__________|

##################################################################

# Function to start a process in a new terminal
start_in_terminal() {
    local title="$1"
    local command="$2"
    
    # Try different terminal emulators
    if command -v gnome-terminal &> /dev/null; then
        gnome-terminal --title="$title" -- bash -c "$command" &
    elif command -v xterm &> /dev/null; then
        xterm -T "$title" -e "$command" &
    elif command -v mate-terminal &> /dev/null; then
        mate-terminal --title="$title" -- "$command" &
    elif command -v lxterminal &> /dev/null; then
        lxterminal -t "$title" -e "$command" &
    elif command -v konsole &> /dev/null; then
        konsole --title "$title" -e "$command" &
    else
        # Fallback to running in background if no terminal is available
        echo "Starting $title in background..."
        $command &
    fi
}

# Start PGCS
start_in_terminal "PGCS" "cd $BASE/cmake-build-debug; ./PGCS $BASE/IO/PGCS/ 1 Intra_Domain -pc Wi-Fi Intra_Domain wlp63s0 e0:e2:e6:00:71:0c 200; exec bash"

# Wait 10 seconds before starting HTS
echo "Waiting 10 seconds before starting HTS..."
sleep 10

# Start HTS
start_in_terminal "HTS" "cd $BASE/cmake-build-debug; ./HTS $BASE/IO/NRNCS/; exec bash"

# Wait 60 seconds before starting IoTTestApp
echo "Waiting 60 seconds before starting IoTTestApp..."
sleep 60

# Start IoTTestApp
start_in_terminal "IoTTestApp" "cd $BASE/cmake-build-debug; ./IoTTestApp IoTTestApp $BASE/IO/IoTTestApp/; exec bash"

echo "All processes have been started in separate terminals."
