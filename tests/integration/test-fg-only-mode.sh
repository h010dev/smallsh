#!/bin/bash

./smallsh <<'___EOF___'
echo --------------------
echo Testing foreground-only mode (20 points for entry & exit text AND ~5 seconds between times)
kill -SIGTSTP $$
date
sleep 5 &
date
kill -SIGTSTP $$
exit
___EOF___
