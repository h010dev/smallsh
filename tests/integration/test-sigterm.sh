#!/bin/bash

./smallsh <<'___EOF___'
echo
echo --------------------
echo sleep 100 background (10 points for returning process ID of sleeper)
sleep 100 &
echo
echo
echo --------------------
echo pkill -signal SIGTERM sleep (10 points for pid of killed process, 10 points for signal)
echo (Ignore message about Operation Not Permitted)
pkill sleep
echo
exit
___EOF___
