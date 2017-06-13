#!/bin/bash

echo "[build.sh] building sources..."
pwd
ls
gcc src/au_cont.c src/au_child.c src/au_utils.c -o bin/au_cont
gcc src/init.c -o bin/init
bin/init
id -r
id -u
echo "[build.sh] sources built"
