#!/bin/bash

# Exit immediately if something fails
set -e

if [[ $(basename "$PWD") != "krunner-veracryptrunner"* ]];then
    git clone https://github.com/alex1701c/krunner-veracryptrunner
    cd krunner-veracryptrunner/
fi

# Add the installation path for the config plugin to the QT_PLUGIN_PATH
if [[ -z "${QT_PLUGIN_PATH}" || "${QT_PLUGIN_PATH}" != *".local/lib/qt/plugins/"* ]]; then
    echo "export QT_PLUGIN_PATH=~/.local/lib/qt/plugins/:$QT_PLUGIN_PATH" >> ~/.bashrc
    export QT_PLUGIN_PATH=~/.local/lib/qt/plugins/:$QT_PLUGIN_PATH
fi

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DQT_PLUGIN_INSTALL_DIR="~/.local/lib/qt/plugins" -DKDE_INSTALL_KSERVICES5DIR="~/.local/share/kservices5" ..
make -j$(nproc)
# Installs config file and plugin in autostart-scripts folder
make install
./bin/veracryptrunner &

kquitapp5 krunner 2> /dev/null
kstart5 --windowclass krunner krunner > /dev/null 2>&1 &

echo "Installation finished !";