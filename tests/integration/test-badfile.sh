#!/bin/bash

./smallsh <<'___EOF___'
echo
echo --------------------
echo wc in badfile (10 points for returning text error)
wc < badfile
echo
echo
echo --------------------
echo badfile (10 points for returning text error)
badfile
echo
exit
___EOF___
