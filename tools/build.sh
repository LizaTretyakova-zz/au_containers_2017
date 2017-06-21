#!/bin/bash

make

sudo chown 0:0 bin/aucont_start
sudo chmod u+s bin/aucont_start
sudo chown 0:0 bin/aucont_stop
sudo chmod u+s bin/aucont_stop
sudo chown 0:0 bin/au_cont
sudo chmod u+s bin/au_cont
sudo chmod +x src/au_setup_workplace.py
sudo chmod +x src/au_setup_network.py
sudo chmod +x src/au_setup_cpu.py
echo "[build.sh] sources built"
