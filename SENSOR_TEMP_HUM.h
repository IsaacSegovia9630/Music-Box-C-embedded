/*
 * SENSOR_TEMP_HUM.h
 *
 *  Created on: 20 oct 2022
 *      Author: Tahirí Ramos
 */

#ifndef EQUIPO_6_P2_SENSOR_TEMP_HUM_H_
#define EQUIPO_6_P2_SENSOR_TEMP_HUM_H_

#include "stdint.h"
#include "MK64F12.h"


#define POW_2_20 			1048576
#define MULT_200			200
#define SUBS_50				50

#define BUSY_BIT			0x10
#define CALIBRATION_BIT		0x08

#define I2C_DATA_LENGTH_SENSOR		6U
#define I2C_MASTER_SLAVE_ADDR_SENSOR_7BIT 	0x38U	//sensor DHT20

#define g_capture_data_length   17

#define mem_adress_RESET_VALUE 	0x00

#define mem_adress2_RESET_VALUE 0x5DF0

typedef float float32_t;

void sensor_restart(void);

void sensor_init(void);

void sensor_read_comands(void);

int32_t read_sensor_hum(void);

int32_t read_sensor_temp(void);

float32_t sensor_humidity(void);

float32_t sensor_temperature(void);

uint8_t get_max_counter_capture(UART_Type *UART);

#endif /* EQUIPO_6_P2_SENSOR_TEMP_HUM_H_ */
