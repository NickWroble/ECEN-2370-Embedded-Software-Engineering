#ifndef SRC_HEADER_FILES_SI7021H_
#define SRC_HEADER_FILES_SI7021H_

//**
// Include files
#include "sleep_routines.h"
#include "em_gpio.h"
#include "em_assert.h"
#include "i2c.h"
#include "brd_config.h"
//**

//**
// structs
//*


//**
// defined files
#define MUL 175.72
#define DIV 65536
#define SUB 46.85
#define FMUL 1.8
#define ADD 32
//**



//**
// global variables
#define I2C_MAX_FREQ 392157
//**

//**
// private function prototypes
//**



//**
// function prototypes;
void si7021_i2c_open();
void si7021_read(I2C_TypeDef *i2c, uint32_t callback);
float si7021_temp();
//**

#endif /* SRC_HEADER_FILES_SI7021H */



