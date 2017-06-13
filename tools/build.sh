#!/bin/bash

make

sudo chown 0:0 bin/aucont_start
sudo chmod u+s bin/aucont_start
sudo chown 0:0 bin/aucont_stop
sudo chmod u+s bin/aucont_stop
sudo chown 0:0 src/aucont_start.py
sudo chmod u+s src/aucont_start.py
sudo chown 0:0 src/aucont_stop.py
sudo chmod u+s src/aucont_stop.py
echo "[build.sh] sources built"
