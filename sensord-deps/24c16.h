#define EEPROM_ADR 0x50

// version 1 contained only ams5915 zero_offset
#define EEPROM_DATA_VERSION 2

// define struct for MS5611 sensor
typedef struct {
	int fd;
	unsigned char address;
} t_24c16;

typedef struct {
	char header[3];
	char data_version;
	char serial[6];
	float zero_offset;
	// added in v2:
	signed int accel_xmin;
	signed int accel_xmax;
	signed int accel_ymin;
	signed int accel_ymax;
	signed int accel_zmin;
	signed int accel_zmax;
	signed int mag_xmin;
	signed int mag_xmax;
	signed int mag_ymin;
	signed int mag_ymax;
	signed int mag_zmin;
	signed int mag_zmax;
	char checksum;
} t_eeprom_data;

// prototypes
int eeprom_open(t_24c16 *, unsigned char);
char eeprom_write(t_24c16 *, char *, unsigned char, unsigned char);
char eeprom_read(t_24c16 *, char *, char, char);
int update_checksum(t_eeprom_data*);
char verify_checksum(t_eeprom_data*);
int eeprom_read_data(t_24c16 *, t_eeprom_data *);



