/*  sensorcal - Sensor Calibration for Openvario Sensorboard - http://www.openvario.org/
    Copyright (C) 2014  The openvario project
    A detailed list of copyright holders can be found in the file "AUTHORS" 

    This program is free software; you can redistribute it and/or 
    modify it under the terms of the GNU General Public License 
    as published by the Free Software Foundation; either version 3
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include "24c16.h"
#include "ams5915.h"
#include "sensorcal.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "mpu9150.h"
#include "linux_glue.h"
#include "../ahrs_settings.h"
	
int g_debug=0;
FILE *fp_console=NULL;
	
int calibrate_ams5915(t_eeprom_data* data)
{
	t_ams5915 dynamic_sensor;
	float offset=0.0;
	int i;
	
	// open sensor for differential pressure
	/// @todo remove hardcoded i2c address for differential pressure
	printf("Open sensor ...");
	if (ams5915_open(&dynamic_sensor, 0x28) != 0)
	{
		printf(" failed !!\n");
		return 1;
	}
	else
	{
		printf(" success!\n");
	}
	
	dynamic_sensor.offset = 0.0;
	dynamic_sensor.linearity = 1.0;
	
	//initialize differential pressure sensor
	ams5915_init(&dynamic_sensor);
	
	for (i=0;i<10 ;i++)
	{
		// read AMS5915
		ams5915_measure(&dynamic_sensor);
		ams5915_calculate(&dynamic_sensor);
		
		// wait some time ...
		usleep(1000000);
		printf("Measured: %f\n",dynamic_sensor.p);
		
		// calc offset 
		offset += dynamic_sensor.p;
	}
		
	data->zero_offset = -1*(offset/10);
	
	return(0);
}

int calibrate_mpu9150(t_eeprom_data* data)
{
	int i2c_bus = AHRS_I2C_BUS;
	int sample_rate = AHRS_SAMPLE_RATE_HZ;
	int i;
	unsigned long loop_delay;
	mpudata_t mpu;
	short minVal[3], change;
	short maxVal[3];
	
	if (sample_rate == 0)
		return 1;
		
	if (mpu9150_init(i2c_bus, sample_rate, 0))
	{
		printf("Failed to connect to mpu9150\n");
		return 1;
	}

	memset(&mpu, 0, sizeof(mpudata_t));

	for (i = 0; i < 3; i++) {
		minVal[i] = 0x7fff;
		maxVal[i] = 0x8000;
	}

	loop_delay = (1000 / sample_rate) - 2;

	printf("Accelerometer calibration\n");
	printf("==========================\n");
	printf("The calibration routine will require you to turn your OpenVario through 360 degrees in three dimensions!\n");
	printf("If you cannot do this, press 'ESC' to cancel now. Once the calibration begins, itmust be completed.\n");
	printf("Press any key to continue, or 'ESC' to cancel\n");
	// TODO prompt
	
	printf("\nRotate your OpenVario gently 360 degrees in three dimensions. Keep turning until you can no longer increase the displayed numbers. When done, press 'ESC'.\n\n");

	linux_delay_ms(loop_delay);
	
	//todo: add loop w/ key interrupt
	while (0) {
		change = 0;

		if (mpu9150_read_dmp(&mpu) == 0) {
			for (i = 0; i < 3; i++) {
				if (mpu.rawAccel[i] < minVal[i]) {
					minVal[i] = mpu.rawAccel[i];
					change = 1;
				}
				if (mpu.rawAccel[i] > maxVal[i]) {
					maxVal[i] = mpu.rawAccel[i];
					change = 1;
				}
			}
		}
	
		if (change) {
			
			data->accel_xmin 	= minVal[0];
			data->accel_xmax 	= maxVal[0];
			data->accel_ymin	= minVal[1];
			data->accel_ymax	= maxVal[1];
			data->accel_zmin	= minVal[2];
			data->accel_zmax	= maxVal[2];
			
			
			printf("\rX %d|%d|%d    Y %d|%d|%d    Z %d|%d|%d             ",
			minVal[0], mpu.rawAccel[0], maxVal[0], 
			minVal[1], mpu.rawAccel[1], maxVal[1],
			minVal[2], mpu.rawAccel[2], maxVal[2]);

			fflush(stdout);
		}

		linux_delay_ms(loop_delay);
	}

	printf("\n\n");

	

	data->mag_xmin		= -250;
	data->mag_xmax		=  250;	
	data->mag_ymin		= -250;
	data->mag_ymax		=  250;
	data->mag_zmin		= -250;
	data->mag_zmax		=  250;
	
	
	mpu9150_exit();
	
	return(0);
}


int main (int argc, char **argv) {
	
	// local variables
	t_24c16 eeprom;
	t_eeprom_data data;
	
	int exit_code=0;
	int result;
	int c;
	int i;
	char zero[1]={0x00};
	

	// usage message
	const char* Usage = "\n"\
	"  -c              calibrate sensors\n"\
	"  -s [serial]     write serial number (6 characters)\n"\
    "  -i              initialize EEPROM. All values will be cleared !!!\n"\
	"\n";
	
	// disable buffering 
	setbuf(stdout, NULL);
	
	// print banner
	printf("sensorcal V%c.%c RELEASE %c build: %s %s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_RELEASE,  __DATE__, __TIME__);
	printf("sensorcal Copyright (C) 2014  see AUTHORS on www.openvario.org\n");
	printf("This program comes with ABSOLUTELY NO WARRANTY;\n");
	printf("This is free software, and you are welcome to redistribute it under certain conditions;\n"); 
	
	// open eeprom object
	result = eeprom_open(&eeprom, 0x50);
	if (result != 0)
	{
		printf("No EEPROM found !!\n");
		exit(1);
	}
		
	// check commandline arguments
	while ((c = getopt (argc, argv, "his:cde")) != -1)
	{
		switch (c) {
			case 'h':
				printf("Usage: sensorcal [OPTION]\n%s",Usage);
				break;
				
			case 'i':
				printf("Initialize EEPROM ...\n");
				for (i=0; i<128; i++)
				{
					result = eeprom_write(&eeprom, &zero[0], i, 1);
				}
				strcpy(data.header, "OV");
				data.data_version = EEPROM_DATA_VERSION;
				strcpy(data.serial, "000000");
				data.zero_offset	=  0.0;
				data.accel_xmin 	= -18000;
				data.accel_xmax 	=  18000;
				data.accel_ymin		= -18000;
				data.accel_ymax		=  18000;
				data.accel_zmin		= -18000;
				data.accel_zmax		=  18000;
				data.mag_xmin		= -250;
				data.mag_xmax		=  250;	
				data.mag_ymin		= -250;
				data.mag_ymax		=  250;
				data.mag_zmin		= -250;
				data.mag_zmax		=  250;
				
				update_checksum(&data);
				printf("Writing data to EEPROM ...\n");
				result = eeprom_write(&eeprom, (char*)&data, 0x00, sizeof(data));
				break;
			
			case 'c':
				// read actual EEPROM values
				printf("Reading EEPROM values ...\n\n");
				if( eeprom_read_data(&eeprom, &data) == 0)
				{
					calibrate_ams5915(&data);
					
					calibrate_mpu9150(&data);
					
					printf("New pressure offset: %f\n",(data.zero_offset));
					
					printf("New accel min X:\t%d\n", data.accel_xmin);
					printf("New accel max X:\t%d\n", data.accel_xmax);
					printf("New accel min Y:\t%d\n", data.accel_ymin);
					printf("New accel max Y:\t%d\n", data.accel_ymax);
					printf("New accel min Z:\t%d\n", data.accel_zmin);
					printf("New accel max Z:\t%d\n", data.accel_zmax);
					printf("New mag min X:\t%d\n", data.mag_xmin);
					printf("New mag max X:\t%d\n", data.mag_xmax);
					printf("New mag min Y:\t%d\n", data.mag_ymin);
					printf("New mag max Y:\t%d\n", data.mag_ymax);
					printf("New mag min Z:\t%d\n", data.mag_zmin);
					printf("New mag max Z:\t%d\n", data.mag_zmax);	
					
					// update EEPROM to latest data version
					data.data_version = EEPROM_DATA_VERSION;				
					
					
					
					update_checksum(&data);
					printf("Writing data to EEPROM ...\n");
					result = eeprom_write(&eeprom, (char*)&data, 0x00, sizeof(data));
				}
				else
				{
					printf("EEPROM content not valid !!\n");
					printf("Please use -i to initialize EEPROM !!\n");
					exit_code=2;
					break;
				}						
				break;
			case 'e':
				// delete complete EEPROM
				printf("Delete whole EEPROM ...\n\n");
				for (i=0; i< sizeof(data); i++)
				{
						result = eeprom_write(&eeprom, &zero[0], 0x00, 1);
				}
				printf("EEPROM cleared !!\n");
				exit_code=3;
				printf("End ...\n");
				exit(exit_code);
				break;
			case 'd':
				// read actual EEPROM values
				printf("Reading EEPROM values ...\n\n");
				if( eeprom_read_data(&eeprom, &data) == 0)
				{
					printf("Actual EEPROM values:\n");
					printf("---------------------\n");
					printf("Serial: \t\t\t%s\n", data.serial);
					printf("Differential pressure offset:\t%f\n",data.zero_offset);
					switch(data.data_version)
					{
						case 1:
							printf("*** No IMU values stored. Please re-calibrate ***\n");
							break;
						case 2:
						case 0:
						default:
							printf("IMU accelerometer min X:\t%d\n", data.accel_xmin);
							printf("IMU accelerometer max X:\t%d\n", data.accel_xmax);
							printf("IMU accelerometer min Y:\t%d\n", data.accel_ymin);
							printf("IMU accelerometer max Y:\t%d\n", data.accel_ymax);
							printf("IMU accelerometer min Z:\t%d\n", data.accel_zmin);
							printf("IMU accelerometer max Z:\t%d\n", data.accel_zmax);
							printf("IMU magnetometer min X:\t%d\n", data.mag_xmin);
							printf("IMU magnetometer max X:\t%d\n", data.mag_xmax);
							printf("IMU magnetometer min Y:\t%d\n", data.mag_ymin);
							printf("IMU magnetometer max Y:\t%d\n", data.mag_ymax);
							printf("IMU magnetometer min Z:\t%d\n", data.mag_zmin);
							printf("IMU magnetometer max Z:\t%d\n", data.mag_zmax);
							break;
					}
						
				}
				else
				{
					printf("EEPROM content not valid !!\n");
					printf("Please use -i to initialize EEPROM !!\n");
					exit_code=2;
					break;
				}
				printf("End ...\n");
				exit(exit_code);
				break;
				
			case 's':
				if( strlen(optarg) == 6)
				{
					// read actual EEPROM values	
					if( eeprom_read_data(&eeprom, &data) == 0)
					{
						for(i=0; i<7;i++)
						{
							data.serial[i]=*optarg;
							optarg++;
						}
						data.serial[7]='\n';
						printf("New Serial number: %s\n",data.serial);
						update_checksum(&data);
						printf("Writing data to EEPROM ...\n");
						result = eeprom_write(&eeprom, (char*)&data, 0x00, sizeof(data));
					}
					else
					{
						printf("EEPROM content not valid !!\n");
						printf("Please use -i to initialize EEPROM !!\n");
						exit_code=2;
						break;
					}
				}
				else
				{
					printf("ERROR: Serialnumber has to have exactly 6 characters !!\n");
					exit_code=1;
					break;
				}
				break;
				
			case '?':
				printf("Unknown option %c\n", optopt);
				printf("Usage: sensorcal [OPTION]\n%s",Usage);
				printf("Exiting ...\n");
		}
	}
		
	printf("End ...\n");
	return(exit_code);
}
	
