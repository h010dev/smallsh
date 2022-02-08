#!/bin/bash

./smallsh <<'___EOF___'
echo
echo --------------------
echo sleep 1 background (10 pts for pid of bg ps when done, 10 for exit value)
sleep 1 &
sleep 1
echo
exit
___EOF___
