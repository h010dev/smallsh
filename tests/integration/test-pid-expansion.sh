#!/bin/bash

./smallsh <<'___EOF___'
echo
echo --------------------
echo mkdir testdir$$
mkdir testdir$$
echo
echo
echo --------------------
echo cd testdir$$
cd testdir$$
echo
echo
echo --------------------
echo pwd (5 points for being in the newly created dir)
pwd
exit
___EOF___
