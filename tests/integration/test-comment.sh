#!/bin/bash

./smallsh <<'___EOF___'
echo BEGINNING TEST SCRIPT
echo
echo --------------------
echo Using comment (5 points if only next prompt is displayed next)
#THIS COMMENT SHOULD DO NOTHING
echo
exit
___EOF___
