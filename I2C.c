/*
 * I2C.c
 *
 *  Created on: 12 oct 2022
 *      Author: Isaac
 */

#include "fsl_port.h"
#include "I2C.h"
#include "GPIO.h"
#include "UART.h"


void I2C_init(void){

	I2C_pins_init();

}
