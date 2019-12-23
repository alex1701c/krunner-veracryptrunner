## VeracryptRunner

# Still in development !

This Krunner plugin allows you to easily mount/unmount configured Veracrypt volumes.
Additionally it provides pass integration.

*This plugin is a rewrite of https://github.com/alex1701c/VeracryptRunner but it uses dbus and is far more optimized*

### Required Dependencies

Debian/Ubuntu:  
`sudo apt install cmake extra-cmake-modules build-essential libkf5runner-dev`  

openSUSE:  
`sudo zypper install cmake extra-cmake-modules libQt5Core5 krunner-devel`  

Fedora:  
`sudo dnf install cmake extra-cmake-modules kf5-krunner-devel`  

Arch (Manjaro):  
`sudo pacman -S cmake extra-cmake-modules`  

### Build instructions  

```
cd /path/to/VeracryptRunner
mkdir build
cd build
cmake ..
make
make install
kquitapp5 krunner 2> /dev/null; kstart5 --windowclass krunner krunner > /dev/null 2>&1 &
```
The `make install` command installs the config file. You have to start the executable manually:  
`./build/bin/VeracryptRunner`  
Or you can start the app using your IDE and for example a debugger.

If you want to install the app you have to configure your project differently:
`cmake -DCMAKE_BUILD_TYPE=Release ..`  
With this the `make install` command will place the executable in the ~/.config/autostart-scripts/ folder and the build is optimized.

