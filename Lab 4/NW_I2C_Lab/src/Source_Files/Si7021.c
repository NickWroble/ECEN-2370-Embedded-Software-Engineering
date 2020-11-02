#include"Si7021.h"
#include"i2c.h"

uint32_t data_read;

void si7021_i2c_open(){
	I2C_OPEN_STRUCT si7021_i2c_open;
	si7021_i2c_open.clhr = i2cClockHLRAsymetric;
	si7021_i2c_open.enable = true;
	si7021_i2c_open.master = false;
	si7021_i2c_open.freq = I2C_MAX_FREQ;
	si7021_i2c_open.refFreq = 0;
	si7021_i2c_open.OUT_ROUTE_SCL = SCLLOC;
	si7021_i2c_open.OUT_ROUTE_SDA = SDALOC;
	si7021_i2c_open.OUT_ROUTE_SCL_EN = true;
	si7021_i2c_open.OUT_ROUTE_SDA_EN = true;
	i2c_open(I2C0, &si7021_i2c_open);
}

void si7021_read(I2C_TypeDef *i2c, uint32_t callback){
	i2c_start(i2c, SLADD, SLREG, callback);
}

float si7021_temp(){
    float c_temp;
    float f_temp;
    read_data(&data_read);// From I2c function
    c_temp = ((MUL*data_read)/DIV)-SUB;
    f_temp = (FMUL *c_temp)+ ADD;
    return f_temp;
}
