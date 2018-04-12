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

#ifndef MPU9150_H
#define MPU9150_H

#include "quaternion.h"

#define MAG_SENSOR_RANGE 	4096
#define ACCEL_SENSOR_RANGE 	32000

typedef struct {
	short offset[3];
	short range[3];
}  t_mpu9150_cal;


typedef struct {
	t_mpu9150_cal accel_cal;
	t_mpu9150_cal mag_cal;
	int rotation;
	float roll_adjust;
	float pitch_adjust;
	float yaw_adjust;
} t_mpu9150;

typedef struct {
	short rawGyro[3];
	short rawAccel[3];
	long rawQuat[4];
	unsigned long dmpTimestamp;

	short rawMag[3];
	unsigned long magTimestamp;

	short calibratedAccel[3];
	short calibratedMag[3];

	quaternion_t fusedQuat;
	vector3d_t fusedEuler;

	float lastDMPYaw;
	float lastYaw;
} mpudata_t;


void mpu9150_set_debug(int on);
int mpu9150_init(int i2c_bus, int sample_rate, int yaw_mixing_factor, int rotation);
void mpu9150_exit();
int mpu9150_read(mpudata_t *mpu);
int mpu9150_read_dmp(mpudata_t *mpu);
int mpu9150_read_mag(mpudata_t *mpu);
void mpu9150_set_accel_cal(t_mpu9150_cal *cal);
void mpu9150_set_mag_cal(t_mpu9150_cal *cal);

int set_orientation(int rotation, signed char gyro_orientation[9]);


#endif /* MPU9150_H */

