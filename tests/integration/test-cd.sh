#!/bin/bash

./smallsh <<'___EOF___'
echo
echo --------------------
echo pwd
pwd
echo
echo
echo --------------------
echo cd
cd
echo
echo
echo --------------------
echo pwd (10 points for being in the HOME dir)
pwd
echo
exit
___EOF___
