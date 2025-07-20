#!/bin/bash
#OliverRunTime=4000
#OliverName=Tokyo Crossing Cam
echo "tokyoCrossing.sh - starting:"
./closeAll.sh
google-chrome https://www.youtube.com/watch?v=DBWgFXSMr-8 &
sleep 3
xdotool key f
echo "tokyoCrossing.sh - exiting."
