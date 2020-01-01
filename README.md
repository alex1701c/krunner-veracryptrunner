## VeracryptRunner

This Krunner plugin allows you to easily mount/unmount configured Veracrypt volumes.
Additionally it provides pass integration.

*This plugin is a rewrite of https://github.com/alex1701c/VeracryptRunner but it uses dbus, is more optimized and configurable*

### Required Dependencies

Debian/Ubuntu:  
`sudo apt install cmake extra-cmake-modules build-essential libkf5runner-dev libkf5notifications-dev`  

openSUSE:  
`sudo zypper install cmake extra-cmake-modules libQt5Core5 krunner-devel knotifications-devel`  

Fedora:  
`sudo dnf install cmake extra-cmake-modules kf5-krunner-devel kf5-knotifications-devel`  

Arch (Manjaro):  
`sudo pacman -S cmake extra-cmake-modules knotifications`  

### Build instructions  

The easiest way to install is:  
`curl https://raw.githubusercontent.com/alex1701c/krunner-veracryptrunner/master/install.sh | bash`  
```
git clone https://github.com/alex1701c/krunner-veracryptrunner
cd krunner-veracryptrunner/
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release  ..
make
make install
kquitapp5 krunner 2> /dev/null; kstart5 --windowclass krunner krunner > /dev/null 2>&1 &
./bin/veracryptrunner &
```
### Screenshots  

#### Show options  
![Show options](https://raw.githubusercontent.com/alex1701c/Screenshots/master/krunner-veracryptrunner/run_options.png)

#### Lauch Veracrypt and fill in password using pass integration  
![Launch Veracrypt with pass integration](https://raw.githubusercontent.com/alex1701c/Screenshots/master/krunner-veracryptrunner/mount_with_pass_integration.gif)

#### Unmount option
![Unmount option](https://raw.githubusercontent.com/alex1701c/Screenshots/master/krunner-veracryptrunner/unmount_option.png)

#### Config dialog  
![Config dialog](https://raw.githubusercontent.com/alex1701c/Screenshots/master/krunner-veracryptrunner/config_overview.png)