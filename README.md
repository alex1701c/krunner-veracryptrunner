## VeracryptRunner

This Krunner plugin allows you to easily mount/unmount configured Veracrypt volumes.
Additionally it provides pass integration.

*This plugin is a rewrite of https://github.com/alex1701c/VeracryptRunner but it uses dbus, is more optimized and configurable*

### Required Dependencies

<details>
<summary><b>Debian/Ubuntu</b></summary>

Plasma5:  
```bash install-ubuntu-plasma5
sudo apt install git cmake extra-cmake-modules build-essential libkf5runner-dev libkf5i18n-dev libkf5kcmutils-dev libkf5dbusaddons-bin
```
Plasma6:  
```bash install-ubuntu-plasma6
sudo apt install git cmake extra-cmake-modules build-essential libkf6runner-dev libkf6i18n-dev libkf6kcmutils-dev libkf6dbusaddons-bin
```

</details>

<details>
<summary><b>OpenSUSE</b></summary>

Plasma5:  
```bash install-opensuse-plasma5
sudo zypper install git cmake extra-cmake-modules ki18n-devel kcmutils-devel knotifications-devel kdbusaddons-tools
```
Plasma6:  
```bash install-opensuse-plasma6
sudo zypper install git cmake kf6-extra-cmake-modules kf6-ki18n-devel kf6-kcmutils-devel kf6-kdbusaddons-tools
```

</details>

<details>
<summary><b>Fedora</b></summary>

Plasma5:  
```bash install-fedora-plasma5
sudo dnf install git cmake extra-cmake-modules kf5-ki18n-devel kf5-kcmutils-devel kf5-knotifications-devel kf5-kdbusaddons
```
Plasma6:  
```bash install-fedora-plasma5
sudo dnf install git cmake extra-cmake-modules kf6-ki18n-devel kf6-kcmutils-devel kf6-knotifications-devel kf6-dbusaddons
```

</details>


### Build instructions  

The easiest way to install is:  
`curl https://raw.githubusercontent.com/alex1701c/krunner-veracryptrunner/master/install.sh | bash`  
```
git clone https://github.com/alex1701c/krunner-veracryptrunner
cd krunner-veracryptrunner/
./install.sh
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
