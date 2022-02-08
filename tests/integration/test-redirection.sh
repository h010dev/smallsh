#!/bin/bash

./smallsh <<'___EOF___'
echo BEGINNING TEST SCRIPT
echo
echo --------------------
echo ls (10 points for returning dir contents)
ls
echo
echo
echo --------------------
echo ls out junk
ls > junk
echo
echo
echo --------------------
echo cat junk (15 points for correctly returning contents of junk)
cat junk
echo
echo
echo --------------------
echo wc in junk (15 points for returning correct numbers from wc)
wc < junk
echo
echo
echo --------------------
echo wc in junk out junk2; cat junk2 (10 points for returning correct numbers from wc)
wc < junk > junk2
cat junk2
echo
exit
___EOF___
