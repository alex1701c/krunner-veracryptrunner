#!/bin/bash

# Exit immediately if something fails
set -e

if [[ $(basename "$PWD") != "krunner-veracryptrunner"* ]];then
    git clone https://github.com/alex1701c/krunner-veracryptrunner
    cd krunner-veracryptrunner/
fi

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
make -j$(nproc)
# Installs config file and plugin in autostart-scripts folder
sudo make install

kquitapp5 krunner 2> /dev/null
kstart5 --windowclass krunner krunner > /dev/null 2>&1 &

echo "Installation finished !";
