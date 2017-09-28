# OpenVario Gliding COmputer - Driver for MPU9150 as AHRS

A work in progress driver for the MPU9150 chip that is currently sitting unused in the [OpenVario Glide Computer](http://www.openvario.org).

Currently, the driver aims to provide output compatible with the XCSoar LevilAHRS driver.

The majority of the code is based on the Linux-MPU9150 sample app by Pansenti. Original source is unavailable, but license file is intact.

Ultimately, once proven, the code will be combined into the OpenVario sensord and sensorcal source trees.

The code uses the InvenSense Embedded Motion Driver v5.1.1 SDK
to obtain fused 6-axis quaternion data from the MPU-9150 DMP. The quaternion
data is then further corrected in the linux-mpu0150 code with magnetometer 
data collected from the IMU.


# Fetch

Use git to fetch the code. Clone this repository.

# Build

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


### local_defaults.h

You can modify some default parameter settings in <code>local_defaults.h</code> to avoid
having to pass command line switches to the applications every run. 

After modifying <code>local_defaults.h</code>, you will have to run <code>make</code> again. 

The defaults in  <code>local_defaults.h</code> are for the RPi.



# Calibration

The IMU gyros have a built in calibration mechanism, but the accelerometers
and the magnetometer require manual calibration.

Calibration, particularly magnetometer calibrations, is a complicated topic.
We've provided a simple utility application called <code>imucal</code> that can get you
started.

        root@openvario:~/# ./imucal -h
         
        Usage: ./imucal <-a | -m> [options]
          -b <i2c-bus>          The I2C bus number where the IMU is. The default is 1 for /dev/i2c-1.
          -s <sample-rate>      The IMU sample rate in Hz. Range 2-50, default 10.
          -a                    Accelerometer calibration
          -m                    Magnetometer calibration
                                Accel and mag modes are mutually exclusive, but one must be chosen.
          -f <cal-file>         Where to save the calibration file. Default ./<mode>cal.txt
          -h                    Show this help
        
        Example: ./imucal -b3 -s20 -a

You'll need to run this utility twice, once for the accelerometers and
again for the magnetometer.

Here is how to generate accelerometer calibration data on an RPi. 
The default bus and sample rate are used.

         root@openvario:~/# ./imucal -a
        
        Initializing IMU .......... done
        
        
        Entering read loop (ctrl-c to exit)
        
        X -16368|16858|16858    Y -16722|-2490|16644    Z -17362|-562|17524             ^C


The numbers shown are min|current|max for each of the axes.

What you want to do is slowly move the RPi/imu through all orientations in
three dimensions. Slow is the the key. We are trying to measure gravity only
and sudden movements will induce unwanted accelerations.

The values will update whenever there is a change in one of the min/max
values, so when you see no more changes you can enter ctrl-c to exit
the program.

When it finishes, the program will create an <code>accelcal.txt</code> file
recording the min/max values.

        root@openvario:~/# cat accelcal.txt 
        -16368
        16858
        -16722
        16644
        -17362
        17524


Do the same thing for the magnetometers running <code>imucal</code> with the -m switch.

        root@openvario:~/# ./imucal -m
        
        Initializing IMU .......... done
        
        
        Entering read loop (ctrl-c to exit)
        
        X -179|-54|121    Y -154|199|199    Z -331|-124|15             ^C


Again move the device through different orientations in all three dimensions
until you stop seeing changes. You can move faster during this calibration
since we aren't looking at accelerations.

After ending the program with ctrl-c, a calibration file called <code>magcal.txt</code>
will be written.

        root@openvario:~/# cat magcal.txt 
        -179
        121
        -154
        199
        -331
        15


If these two files, <code>accelcal.txt</code> and <code>magcal.txt</code>, are left in the
same directory as the <code>imu</code> program, they will be used by default.


# Run

The <code>ahrsd</code> application is intended to be run as a daemon. Currently you will have to run it under a separate console (e.g. via SSH) while OpenVario is in the foreground.

The defaults will work for the OpenVario with the two calibration files picked
up automatically, but you can see the available options with <code>ahrsd -h</code>

----

All of the functions in the Invensense SDK under the <code>eMPL</code> directory
are available. See <code>mpu9150/mpu9150.c</code> for some examples.

