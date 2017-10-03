# OpenVario Gliding Computer - Driver for MPU9150 as AHRS

A work in progress driver for the MPU9150 chip that is currently sitting unused in the [OpenVario Glide Computer](http://www.openvario.org).

Currently, the driver aims to provide output compatible with the XCSoar LevilAHRS driver.


Ultimately, once proven, the code will be combined into the OpenVario sensord and sensorcal source trees.



# Getting the driver

You can either compile the code yourself, or just install a pre-built binary.

To compile yourself, use git to fetch the code. Clone this repository. Then jump to "Compiling the driver" below.

To install a pre-built binary, grab [ahrsd](http://glidist.jfwhome.com/ahrsd) and [imucal](http://glidist.jfwhome.com/imucal) and jump to "Installation" below.


# Compiling the driver

There will be three ways to compile and test the driver.

1. Build the file as a recipe as part of the Openvario build system, then test the resultant image. This is not available yet.
2. Cross-compile the code making reference to the OpenVario tool chain, and transfer the resultant files to your OpenVario.
3. Build the file on a Linux desktop/laptop computer. This is only really for developers who want to modify the code and test that it compiles, as the resultant programmes will only run on the machine you compile it on, and are unlikely to do anything other than tell you that the MPU9150 IMU is unavailable.

Option (2) uses <code>Makefile-cross</code>. Option (3) uses <code>Makefile-native</code>.

For Options (1) and (2), remember to source the environment file for the OpenEmbedded toolchain. The exact command will depend on where the OpenVario build system is installed, but will be something like:

        user@mydesktop:~$ source ../rootfs/environment-xxxxxxx

A recommendation is to create a symbolic link to the make file you want to use:

        user@mydesktop:~$ cd openvario-ahrs
        user@mydesktop:~/openvario-ahrs$ ln -s Makefile-native Makefile

After that you can just type <code>make</code> to build the code.

The result is two executables called <code>ahrsd</code> and <code>imucal</code>.


# Installation

Copy ahrs and imucal anywhere on your OpenVario for testing. For example, for now, /home/root is fine.

The programs need to know the installed orientation of your openvario in your panel. 

They get this by reading the <code>/boot/config.uEnv</code> file. If you don't have one, create it,
and add the following to it:

<code>rotation=X</code>

Where X is a number 0 - 3:
0 = Normal landscape
1 = Portrait, rotated 90 degrees
2 = Landscape, upside down
3 = Portrait, rotated 270 degrees



# Calibration

You'll need to calibrate the accelerometer to use it. 
Run <code>./imucal -a</a> and slowly move your openvario through all
orientations in three dimensions. Slowly is the the key. 
We are trying to measure gravity only and sudden movements will 
induce unwanted accelerations.

The values will update whenever there is a change in one of the min/max
values, so when you see no more changes you can enter ctrl-c to exit
the program.

When it finishes, the program will create an <code>accelcal.txt</code> file
recording the min/max values.

Do the same for the magnetometer by running <code>imucal -m</code>. After ending the program with ctrl-c, 
a calibration file called <code>magcal.txt</code>
will be written.

Leave the two calibration files, <code>accelcal.txt</code> and <code>magcal.txt</code>, in the
same directory as the <code>ahrsd</code> program, they will be used by default.


# Run

For now you have to run ahrsd manually. Exit to the shell and run <code>nohup ./ahrsd &</code>, 
then restart XCSoar with <code>/opt/bin/ovmenu-ng.sh</code>. 
Alternatively, you might find it easiest to run it under a separate console (e.g. via SSH) 
while XCSoar is in the foreground.

<code>./ahrsd</code>

In XCSoar, add a new device: 
Port = TCP Port
TCP Port = 2000
Driver = Levil AHRS


# Copyright

The majority of the code is based on the Linux-MPU9150 sample app by Pansenti. 
Original source is unavailable, but license file is intact.

The code uses the InvenSense Embedded Motion Driver v5.1.1 SDK
to obtain fused 6-axis quaternion data from the MPU-9150 DMP. The quaternion
data is then further corrected in the linux-mpu0150 code with magnetometer 
data collected from the IMU.

