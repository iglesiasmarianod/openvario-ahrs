# OpenVario Glide Computer - Driver for MPU9150 as AHRS
John Wells


A driver for the MPU9150 chip that is currently unused in the [OpenVario Glide Computer](http://www.openvario.org).

The driver provides output compatible with the XCSoar LevilAHRS driver on TCP port 2000.



# Getting the driver

You can either compile the code yourself, or just install a pre-built binary.

To compile yourself, use git to fetch the code. Clone this repository. Then jump to "Compiling the driver" below.

To install a pre-built binary, grab [sensord](http://glidist.com/temp/sensord) and [sensorcal](http://glidist.com/temp/sensorcal) and jump to "Installation" below.


# Compiling the driver

The driver is incorporated into sensord, and will compile with 
<code>bitbake sensord</code> as usual. If you want to build out of tree, 
you can cross-compile the code making reference to the 
OpenVario tool chain, using <code>Makefile-temp-cross</code>:

First source the environment file for the OpenEmbedded toolchain. 
The exact command will depend on where the OpenVario build system is installed, 
but will be something like:

        user@mydesktop:~$ source ../rootfs/environment-xxxxxxx

Then make sensord with:

        user@mydesktop:~$ make -f Makefile-temp-cross


# Installation

Copy sensord and sensorcal to your /opt/bin directory. To do this, you will need to 
ensure that sensord is not running, and then remount your file system read-write:

	root@mopenvario:~# mount -o remount,rw /
	
You will also need to update your /opt/conf/sensord.conf file to include 
the new settings. You could copy across the version provided here, but 
that would overwtite your changed settings, such as your voltage calibration value. 
Instead, copy the new  settings in the sensord.conf file here over to your file and adjust as needed.

The most important option in sensord.conf is the orientation, set in 
mpu_rotation. The programs need to know the installed orientation of 
your openvario in your panel. 

<code>mpu_rotation X</code>

Where X is a number 0 - 3:
* 0 = Normal landscape
* 1 = Portrait, rotated 90 degrees
* 2 = Landscape, upside down
* 3 = Portrait, rotated 270 degrees

Note that if your openvario is non-standard (e.g. the screen is stuck on upside down), 
you may have to try a different number.

The other new options in sensord.conf (roll_adjust, pitch_adjust and yaw_adjust) are just to make 
final "tweaks" to orientation, if for example, your sensor board is 
installed slightly wonky. Values should be at most one or two degrees, and for most people should be zero.



# Initialisation and Calibration

You'll need to calibrate the accelerometer and magnetometer before using them for the first time.

To do this, you can run sensorcal. Sensorcal stores calibration data in the EEPROM, 
which will need to be re-initialised. 
The full set of commands will look something like the below (refer to 
the Openvario documentation for full details on the EEPROM):

	root@mopenvario:~# sensorcal -i
	root@mopenvario:~# sensorcal -s XXXXXX
	root@mopenvario:~# sensorcal -c

The first command erases the EEPROM. The second re-initialises it with a custom serial number. Replace 'XXXXXX' with any six-digit code. The third runs the calibration.	

Run through all calibration options (you will need to recalibrate your 
pressure sensors). When prompted, slowly move your openvario through all
orientations in three dimensions. Slowly is the the key. 
We are trying to measure gravity only and sudden movements will 
induce unwanted accelerations.

The values will update whenever there is a change in one of the min/max
values, so when you see no more changes hit ESC to move on.

You will be asked to repeat the process for both the accelerometer and the magnetometer. 


# Running

The driver will run automatically as part of sensord.

In XCSoar, add a new device: 
* Port = TCP Port
* TCP Port = 2000
* Driver = Levil AHRS

You'll also need to add an AHRS screen to your XCSoar screen/layout profile.


# Copyright

The MPU9150 driver layer code is based on the Linux-MPU9150 sample app by Pansenti. 
Original source is unavailable, but license file is intact.

The code uses the InvenSense Embedded Motion Driver v5.1.1 SDK
to obtain fused 6-axis quaternion data from the MPU-9150 DMP. The quaternion
data is then further corrected in the linux-mpu0150 code with magnetometer 
data collected from the IMU.

