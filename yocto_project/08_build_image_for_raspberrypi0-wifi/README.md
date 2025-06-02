# Yocto Project Image for Raspberry Pi Zero W

[![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
![Platform](https://img.shields.io/badge/Platform-Linux-lightgrey)

## Table of Contents
- [About This Implementation](#about-this-implementation)
- [Introduction](#introduction)
- [Preparing The Environment](#preparing-the-environment)
- [Yocto Environment Setup](#yocto-environment-setup)
- [Build and Flash Image](#build-and-flash-image)
- [Conclusion](#conclusion)

## About This Implementation

**This is a direct adaptation  for educational purposes of two tutorials [Run Image Emulation Using QEMU](https://www.devlinux.vn/blog/Ch%E1%BA%A1y-gi%E1%BA%A3-l%E1%BA%ADp-image-b%E1%BA%B1ng-QEMU) and [Build Image For Raspberry Pi Zero W](https://www.devlinux.vn/blog/Build-Image-cho-Raspberry-Pi-Zero-W) from [DevLinux.vn](https://www.devlinux.vn/).**  
All instructions and configurations are based on their original work with minor adjustments for clarity.

## Introduction

The Yocto Project is a powerful open-source collaboration tool that enables developers to create custom Linux systems for embedded devices. This repository contains the configuration and instructions for building a complete Linux image tailored for the **Raspberry Pi Zero W** - one of the most popular and versatile development boards available today.

## Preparing The Environment

### Hardware and Software Requirements

Before starting, ensure you have:
- **Host Machine**: Ubuntu (recommended on VMware) with at least 100GB free space
- **Target Device**: Raspberry Pi Zero W
- **Storage**: Minimum 16GB microSD card
- **Internet Connection**: Required for downloading packages and source code

### Installing Required Packages
First, install essential packages on the Ubuntu host:
```bash
sudo apt install gawk wget git diffstat unzip texinfo gcc build-essential chrpath socat \
cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping \
python3-git python3-jinja2 python3-subunit zstd liblz4-tool file locales libacl1
```

## Yocto Environment Setup

### Download Yocto Project Source Code
Create and navigate to the **~/yocto** directory:
```bash
mkdir ~/yocto && cd ~/yocto
```
Download the Yocto Project source code from the official repository:
``` bash
~/yocto$ git clone git://git.yoctoproject.org/poky
```
### Yocto Directory Structure
After successful download, the Poky directory structure will contain:
| **Directory**           | **Purpose**                                               |
|-------------------------|-----------------------------------------------------------|
| ```bitbake/```          | BitBake tool - Core build system manager                  |
| ```meta/```             | Base layer containing core recipes, classes and functions |
| ```meta-poky/```        | Poky extension layer with default configurations          |
| ```meta-yocto-bsp/```   | Board Support Packages (BSP) for specific hardware        |
| ```scripts/```          | Project management support scripts                        |
| ```oe-init-build-env``` | Environment setup script                                  |

### Switch to Target Branch
Ensure you're in the Poky source directory and check out the appropriate branch:
```bash
cd ~/yocto/poky
git checkout dunfell
```

### Download the meta-raspberrypi Layer

Download the Raspberry Pi-specific layer:
```bash
~/yocto/poky$ git clone git://git.yoctoproject.org/meta-raspberrypi -b dunfell
```

### Download the meta-openembedded Layer 
Fetch additional essential layers:
```bash 
~/yocto/poky$ git clone git://git.openembedded.org/meta-openembedded -b dunfell
```

### Initialize Build Environment
Run this command to set up necessary environment variables:
```bash
~/yocto/poky$ source oe-init-build-env
```
After executing this command, a build directory will be created. This directory will contain configuration files and build outputs.

### Build Configuration

#### Modify local.conf
The **local.conf** file is a local configuration file generated when initializing the build environment using the **oe-init-build-env** script. Located in **poky/build/conf/**, this file controls critical build parameters for your Yocto image.

Open **conf/local.conf** and set the machine configuration for **Raspberry Pi Zero W**:
```bash
# MACHINE ??= "qemux86-64"
MACHINE ??= "raspberrypi0-wifi"
```
For **disk space optimization**, add this line to clean temporary work directories post-build:
```bash
INHERIT += "rm_work"
```

For **package manager selection**, set your preferred package manager (default: RPM):
```bash
PACKAGE_CLASSES ?= "package_rpm"
```

#### Add Meta Layers to bblayers.conf
The **bblayers.conf** file manages the list of metadata layers that BitBake will use during the build process.

Open **conf/bblayers.conf** and update the **BBLAYERS** variable to include:
```bash
BBLAYERS ?= " \
  /home/khang/working_space/yocto/poky/meta \
  /home/khang/working_space/yocto/poky/meta-poky \
  /home/khang/working_space/yocto/poky/meta-yocto-bsp \
  /home/khang/working_space/yocto/poky/meta-devlinux \
  /home/khang/working_space/yocto/poky/build/workspace \
  /home/khang/working_space/yocto/poky/meta-raspberrypi \
  /home/khang/working_space/yocto/poky/meta-openembedded/meta-oe \
  "
```
**Note**: Adjust paths according to your directory structure.

## Build and Flash Image

### Start the Build Process
```bash
~/yocto/poky$ bitbake core-image-sato
```
**Note**: 
- The first build may take 30 minutes to several hours depending on your system specs:
    - CPU cores
    - RAM capacity
    - Internet connection speed
    - Other factors
- Subsequent builds will be faster as many parts of the build process are cached.
### Flash Image to SD Card
**Note**: This step stops at image preparation as no physical SD card is involved.

#### Locate Generated Image
After successful build, find the image at:
```bash
~/yocto/poky/build/tmp/deploy/images/raspberrypi0-wifi/
```

#### Create and Extract Image Copy
``` bash
~/yocto/poky$ cp build/tmp/deploy/images/raspberrypi0-wifi/core-image-minimal-raspberrypi0-wifi-20241123012031.rootfs.wic.bz2 raspberrypi0-wifi.wic.bz2
~/yocto/poky$ bzip2 -d raspberrypi0-wifi.wic.bz2
```
**Note**: Always verify the exact image filename in your tmp/deploy/images/raspberrypi0-wifi/ directory before executing this command. File names may vary based on:
- Your chosen image recipe (e.g., core-image-base)
- Yocto Project version
- Custom configurations

## Conclusion
Building a custom Yocto image for Raspberry Pi Zero W offers:
- **High Customization**: Add/remove packages as needed
- **Optimization**: Compact and efficient output
- **Full Control**: Complete visibility of system components

This implementation successfully demonstrates custom image creation for Raspberry Pi Zero W, providing a foundation for future IoT and embedded projects.
