////////////////////////////////////////////////////////////////////////////
//
//  This file is part of linux-mpu9150
//
//  Copyright (c) 2013 Pansenti, LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of 
//  this software and associated documentation files (the "Software"), to deal in 
//  the Software without restriction, including without limitation the rights to use, 
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
//  Software, and to permit persons to whom the Software is furnished to do so, 
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all 
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef LOCAL_DEFAULTS_H
#define LOCAL_DEFAULTS_H

// To avoid having to pass the same command line switches when running
// the test apps, you can specify the defaults for your platform here.

// Openvario i2c bus
#define DEFAULT_I2C_BUS 1


// platform independent

#define DEFAULT_SAMPLE_RATE_HZ	10

#define DEFAULT_YAW_MIX_FACTOR 4



	/**
	 * MPU is orientated with pin1 away from pressure inlets 
	 * (i.e. away from dir of travel on x axis)
	 * y axis is towards cinch socket, which is towards right wing in "normal" orientation
	 * z axis is upside down in "normal" orientation, 
	 * 
	 * ***NOTE gyro not oriented to pin1 internally, mag & accel orientations differ ***
	 */
	     /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */
	signed char gyro_orientation[9] = { 1, 0, 0,
                                        0, 1, 0,
                                        0, 0, 1 };



#endif /* LOCAL_DEFAULTS_H */

