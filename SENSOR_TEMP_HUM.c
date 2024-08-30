/*
 * SENSOR_TEMP_HUM.c
 *
 *  Created on: 20 oct 2022
 *      Author: Tahirí Ramos
 */

#include "SENSOR_TEMP_HUM.h"
#include "I2C.h"
#include "UART.h"
#include "fsl_i2c.h"
#include "PIT.h"
#include "bits.h"
#include "MEM.h"
#include "RTC.h"

uint8_t g_master_txBuff_sensor[I2C_DATA_LENGTH_SENSOR];
uint8_t g_master_rxBuff_sensor[I2C_DATA_LENGTH_SENSOR];

uint8_t deviceAddress_sensor = 0;
i2c_master_transfer_t masterXfer_sensor;

//error es verdadero, sin error es falso
uint8_t error_sensor_flag = TRUE;

static uint8_t g_capture_counter1 = 0;
static uint8_t g_capture_counter_max1 = 0;

static uint8_t g_capture_counter2 = 0;
static uint8_t g_capture_counter_max2 = 0;

static uint16_t mem_adress = mem_adress_RESET_VALUE;

static uint16_t mem_adress2 = mem_adress2_RESET_VALUE; // 0x5DF0 = 24,048 en hexadecimal

struct temp
{
    uint8_t temp_tens;
    uint8_t temp_units;
    uint8_t temp_first_decimal;
    uint8_t temp_second_decimal;
}temp_digits;

static uint8_t g_capture[g_capture_data_length];

static uint8_t g_capture2[g_capture_data_length];

void sensor_restart(void)
{
	// comando de reinicio
	g_master_txBuff_sensor[0] = 0xAB;

	masterXfer_sensor.slaveAddress   = I2C_MASTER_SLAVE_ADDR_SENSOR_7BIT;
	masterXfer_sensor.direction      = kI2C_Write;
	masterXfer_sensor.subaddress     = (uint32_t)deviceAddress_sensor;
	masterXfer_sensor.subaddressSize = ZERO;
	masterXfer_sensor.data           = g_master_txBuff_sensor;
	masterXfer_sensor.dataSize       = 1;
	masterXfer_sensor.flags          = kI2C_TransferDefaultFlag;
	I2C_MasterTransferBlocking(I2C0, &masterXfer_sensor);

}

void sensor_init(void)
{
	g_master_txBuff_sensor[ZERO] = 0xE1;
	g_master_txBuff_sensor[ONE] = 0x08;
	g_master_txBuff_sensor[TWO] = 0x00;

	masterXfer_sensor.slaveAddress   = I2C_MASTER_SLAVE_ADDR_SENSOR_7BIT;
	masterXfer_sensor.direction      = kI2C_Write;
	masterXfer_sensor.subaddress     = (uint32_t)deviceAddress_sensor;
	masterXfer_sensor.subaddressSize = ZERO;
	masterXfer_sensor.data           = g_master_txBuff_sensor;
	masterXfer_sensor.dataSize       = 3;
	masterXfer_sensor.flags          = kI2C_TransferDefaultFlag;
	I2C_MasterTransferBlocking(I2C0, &masterXfer_sensor);

	masterXfer_sensor.slaveAddress   = I2C_MASTER_SLAVE_ADDR_SENSOR_7BIT;
	masterXfer_sensor.direction      = kI2C_Read;
	masterXfer_sensor.subaddress     = (uint32_t)deviceAddress_sensor;
	masterXfer_sensor.subaddressSize = ZERO;
	masterXfer_sensor.data           = g_master_rxBuff_sensor;
	masterXfer_sensor.dataSize       = 1;
	masterXfer_sensor.flags          = kI2C_TransferDefaultFlag;
	I2C_MasterTransferBlocking(I2C0, &masterXfer_sensor);

	//Leer registro de estado hasta que el bit de calibración tenga un 1 lógico
	if(TRUE == CALIBRATION_BIT && g_master_rxBuff_sensor[0])
	{
		error_sensor_flag = FALSE;
	}
	else
	{
		error_sensor_flag = TRUE;
	}

}

void sensor_read_comands(void)
{
	g_master_txBuff_sensor[0] =  0xAC;
    g_master_txBuff_sensor[1] =  0x33;
    g_master_txBuff_sensor[2] =  0x00;

    masterXfer_sensor.slaveAddress   = I2C_MASTER_SLAVE_ADDR_SENSOR_7BIT;
	masterXfer_sensor.direction      = kI2C_Write;
	masterXfer_sensor.subaddress     = (uint32_t)deviceAddress_sensor;
	masterXfer_sensor.subaddressSize = ZERO;
	masterXfer_sensor.data           = g_master_txBuff_sensor;
	masterXfer_sensor.dataSize       = 3;
	masterXfer_sensor.flags          = kI2C_TransferDefaultFlag;
	I2C_MasterTransferBlocking(I2C0, &masterXfer_sensor);

    /* subAddress = 0x01, data = g_master_rxBuff - read from slave.
      start + slaveaddress(w) + subAddress + repeated start + slaveaddress(r) + rx data buffer + stop */
    masterXfer_sensor.slaveAddress   = I2C_MASTER_SLAVE_ADDR_SENSOR_7BIT;
    masterXfer_sensor.direction      = kI2C_Read;
    masterXfer_sensor.subaddress     = (uint32_t)deviceAddress_sensor;
    masterXfer_sensor.subaddressSize = ZERO;
    masterXfer_sensor.data           = g_master_rxBuff_sensor;
    masterXfer_sensor.dataSize       = 1;
    masterXfer_sensor.flags          = kI2C_TransferDefaultFlag;
    I2C_MasterTransferBlocking(I2C0, &masterXfer_sensor);

    //comparación eer registro de estatus hasta que el bit Busy indication cambie un 1 lógico bit[7]
    if(TRUE == BUSY_BIT && g_master_rxBuff_sensor[0])
    {
    	error_sensor_flag = FALSE;
    }
    else
    {
    	error_sensor_flag = TRUE;
    }
}

int32_t read_sensor_hum(void)
{
	int32_t state = 0;

    masterXfer_sensor.slaveAddress   = I2C_MASTER_SLAVE_ADDR_SENSOR_7BIT;
    masterXfer_sensor.direction      = kI2C_Read;
    masterXfer_sensor.subaddress     = (uint32_t)deviceAddress_sensor;
    masterXfer_sensor.subaddressSize = ZERO;
    masterXfer_sensor.data           = g_master_rxBuff_sensor;
    masterXfer_sensor.dataSize       = I2C_DATA_LENGTH_SENSOR;
    masterXfer_sensor.flags          = kI2C_TransferDefaultFlag;
    state = I2C_MasterTransferBlocking(I2C0, &masterXfer_sensor);

	float32_t digital_humidity = 0;
	float32_t humidity = 0;

	digital_humidity = g_master_rxBuff_sensor[1] << 16;
	digital_humidity += g_master_rxBuff_sensor[2] << 8;
	digital_humidity += (g_master_rxBuff_sensor[3] & 0xF0);

	humidity = digital_humidity/POW_2_20;

	uint8_t humidity2 = (uint8_t)humidity + ASCII_OFFSET;

	g_capture[4] = humidity2;

	update_hum(humidity2);

	return state;
}

int32_t read_sensor_temp(void)
{
	int32_t state = 0;

    masterXfer_sensor.slaveAddress   = I2C_MASTER_SLAVE_ADDR_SENSOR_7BIT;
    masterXfer_sensor.direction      = kI2C_Read;
    masterXfer_sensor.subaddress     = (uint32_t)deviceAddress_sensor;
    masterXfer_sensor.subaddressSize = ZERO;
    masterXfer_sensor.data           = g_master_rxBuff_sensor;
    masterXfer_sensor.dataSize       = I2C_DATA_LENGTH_SENSOR;
    masterXfer_sensor.flags          = kI2C_TransferDefaultFlag;
    state = I2C_MasterTransferBlocking(I2C0, &masterXfer_sensor);

	float32_t digital_temperature = 0;
	float32_t temperature = 0;

	digital_temperature = (g_master_rxBuff_sensor[THREE] & 0x07) << 16;
	digital_temperature += g_master_rxBuff_sensor[FOUR] << 8;
	digital_temperature += g_master_rxBuff_sensor[FIVE];

	temperature = ((digital_temperature/POW_2_20) * MULT_200) - SUBS_50;

	temperature = temperature*100;

	int temperature2 = (int)temperature % 10;

	temp_digits.temp_second_decimal =  (uint8_t)temperature2 + ASCII_OFFSET;

	temperature = temperature/10;

	temperature2 = (int)temperature % 10;

	temp_digits.temp_first_decimal =  (uint8_t)temperature2 + ASCII_OFFSET;

	temperature = temperature/10;

	temperature2 = (int)temperature % 10;

	temp_digits.temp_units =  (uint8_t)temperature2 + ASCII_OFFSET;

	temperature = temperature/10;

	temperature2 = (int)temperature % 10;

	temp_digits.temp_tens =  (uint8_t)temperature2 + ASCII_OFFSET;

	g_capture[0] = temp_digits.temp_tens;
	g_capture[1] = temp_digits.temp_units;
	g_capture[2] = temp_digits.temp_first_decimal;
	g_capture[3] = temp_digits.temp_second_decimal;

	update_temp(temp_digits.temp_tens,temp_digits.temp_units,temp_digits.temp_first_decimal,temp_digits.temp_second_decimal);

	return state;
}

void data_capture1(void)
{
	if(g_capture_counter1 == g_capture_counter_max1)
	{
		pit_ch1_stop();
		g_capture_counter1 = ZERO;
		mem_adress = mem_adress_RESET_VALUE;
	}
	else
	{
		g_capture_counter1++;
		read_sensor_temp();
		read_sensor_hum();
		struct number_time date_data = RTC_read_device_log();

		g_capture[5] = date_data.teen_hours;
		g_capture[6] = date_data.hours;
		g_capture[7] = date_data.teen_minutes;
		g_capture[8] = date_data.minutes;
		g_capture[9] = date_data.teen_seconds;
		g_capture[10] = date_data.seconds;

		g_capture[11] = date_data.teen_day;
		g_capture[12] = date_data.day;
		g_capture[13] = date_data.teen_month;
		g_capture[14] = date_data.month;
		g_capture[15] = date_data.teen_year;
		g_capture[16] = date_data.year;

		write_mem(g_capture, mem_adress, g_capture_data_length);

		mem_adress = mem_adress + g_capture_data_length;

	}
}

void data_capture2(void)
{

	if(g_capture_counter2 == g_capture_counter_max2)
	{
		pit_ch2_stop();
		g_capture_counter2 = ZERO;
		mem_adress2 = mem_adress2_RESET_VALUE;
	}
	else
	{
		g_capture_counter2++;
		read_sensor_temp();
		read_sensor_hum();
		struct number_time date_data = RTC_read_device_log();

		g_capture2[5] = date_data.teen_hours;
		g_capture2[6] = date_data.hours;
		g_capture2[7] = date_data.teen_minutes;
		g_capture2[8] = date_data.minutes;
		g_capture2[9] = date_data.teen_seconds;
		g_capture2[10] = date_data.seconds;

		g_capture2[11] = date_data.teen_day;
		g_capture2[12] = date_data.day;
		g_capture2[13] = date_data.teen_month;
		g_capture2[14] = date_data.month;
		g_capture2[15] = date_data.teen_year;
		g_capture2[16] = date_data.year;

		write_mem(g_capture2, mem_adress2, g_capture_data_length);

		mem_adress = mem_adress + g_capture_data_length;

	}
}

void set_max_data_capture(UART_Type *UART, uint8_t capture)
{
	if(UART == UART0)
	{
		g_capture_counter_max1 = capture;
	}
	if(UART == UART4)
	{
		g_capture_counter_max2 = capture;
	}
}

uint8_t get_max_counter_capture(UART_Type *UART)
{
	if(UART == UART0)
	{
		return g_capture_counter_max1;
	}
	if(UART == UART4)
	{
		return g_capture_counter_max2;
	}

	return ZERO;
}


float32_t sensor_humidity(void)
{
	float32_t digital_humidity = 0;
	float32_t humidity = 0;

    masterXfer_sensor.slaveAddress   = I2C_MASTER_SLAVE_ADDR_SENSOR_7BIT;
    masterXfer_sensor.direction      = kI2C_Read;
    masterXfer_sensor.subaddress     = (uint32_t)deviceAddress_sensor;
    masterXfer_sensor.subaddressSize = ZERO;
    masterXfer_sensor.data           = g_master_rxBuff_sensor;
    masterXfer_sensor.dataSize       = I2C_DATA_LENGTH_SENSOR;
    masterXfer_sensor.flags          = kI2C_TransferDefaultFlag;
    I2C_MasterTransferBlocking(I2C0, &masterXfer_sensor);

	//read_sensor();
	digital_humidity = g_master_rxBuff_sensor[1] << 16;
	digital_humidity += g_master_rxBuff_sensor[2] << 8;
	digital_humidity += (g_master_rxBuff_sensor[3] & 0xF0);

	humidity = digital_humidity/POW_2_20;
	return humidity;
}

float32_t sensor_temperature(void)
{
	float32_t digital_temperature = 0;
	float32_t temperature = 0;

    masterXfer_sensor.slaveAddress   = I2C_MASTER_SLAVE_ADDR_SENSOR_7BIT;
    masterXfer_sensor.direction      = kI2C_Read;
    masterXfer_sensor.subaddress     = (uint32_t)deviceAddress_sensor;
    masterXfer_sensor.subaddressSize = ZERO;
    masterXfer_sensor.data           = g_master_rxBuff_sensor;
    masterXfer_sensor.dataSize       = I2C_DATA_LENGTH_SENSOR;
    masterXfer_sensor.flags          = kI2C_TransferDefaultFlag;
    I2C_MasterTransferBlocking(I2C0, &masterXfer_sensor);

	digital_temperature = (g_master_rxBuff_sensor[3] & 0x07) << 16;
	digital_temperature += g_master_rxBuff_sensor[4] << 8;
	digital_temperature += g_master_rxBuff_sensor[5];

	temperature = ((digital_temperature/POW_2_20) * MULT_200) - SUBS_50;

	return temperature;
}
