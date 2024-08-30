/*
 * RTC.c
 *
 *  Created on: 24 oct 2022
 *      Author: isaac
 */

#include "RTC.h"
#include "UART.h"
#include "I2C.h"
#include "bits.h"
#include "fsl_i2c.h"

uint8_t g_master_txBuff[I2C_DATA_LENGTH];
uint8_t g_master_rxBuff[I2C_DATA_LENGTH];
uint8_t g_master_rxBuff_log[I2C_DATA_LENGTH];

const uint8_t BIT_12_MODE = 0x20;
const uint8_t BIT_24_MODE = 0x00;

const uint8_t INITIAL_SEC   = 0x00;
const uint8_t INITIAL_MIN   = 0x00;
const uint8_t INITIAL_HOUR  = 0x12;
const uint8_t INITIAL_DAY   = 0x27;
const uint8_t INITIAL_MONTH = 0x10;
const uint8_t INITIAL_YEAR  = 0x22;
const uint8_t INITIAL_SQWE  = 0x10;

static uint8_t g_READ_HR_state = FALSE;
static uint8_t g_UART_state = INICIALIZATION_VALUE;

/* subAddress = 0x01, data = g_master_txBuff - write to slave.
  start + slaveaddress(w) + subAddress + length of data buffer + data buffer + stop*/
uint8_t deviceAddress     = 0;

struct time received_data_time;

struct number_time received_data_time_log;

void RTC_init(void)
{
	i2c_master_config_t masterConfig;
	uint32_t sourceClock;
	i2c_master_transfer_t masterXfer;

    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = I2C_BAUDRATE;

    sourceClock = I2C_MASTER_CLK_FREQ;

    I2C_MasterInit(EXAMPLE_I2C_MASTER_BASEADDR, &masterConfig, sourceClock);

    //Pone en 0 la estructura de transferencia
    memset(&masterXfer, 0, sizeof(masterXfer));

    //Seconds 00-59
    g_master_txBuff[Sec]= INITIAL_SEC; //

    //Minutes 00-59
    g_master_txBuff[Min]= INITIAL_MIN; //0100 0101;In big endian, the firsts four bits is a 5 for the hours. The other 4 is a 4 for the tens of minutes

    //Hours 1-12 or 00-23
    g_master_txBuff[Hour]= INITIAL_HOUR | BIT_12_MODE; //11 0010 ;In big endian, the firsts four bits is a 2 for the hours. The 5 bit is one for teen hours
    							// The 6 bit is to set the 12 hour mode with 1 (0 is for 24 hour mode)

    //Number of the Day 01-31
    g_master_txBuff[Day]= INITIAL_DAY;

    //Month 01-12
    g_master_txBuff[Month]= INITIAL_MONTH;

    //Year 00-99
    g_master_txBuff[Year]= INITIAL_YEAR;

    //SQWE Function
    g_master_txBuff[SQWE]=INITIAL_SQWE;

    //Escribir en el RTC
    //Que slave quiero, dirección a 7 bits
    masterXfer.slaveAddress   = I2C_MASTER_SLAVE_ADDR_7BIT_RTC;
    //Si quiero leer o escribir
    masterXfer.direction      = kI2C_Write;
    //subaddress = Se refiere al registro interno al que queremos afectar
    //Si queremos segundos se pone 0, si queremos minutos 1, si queremos horas 2
    masterXfer.subaddress     = (uint32_t)deviceAddress;
    //Que tan grande es en bytes los valores que queremos afectar (memoria = 2, RTC = 1)
    masterXfer.subaddressSize = ONE;
    masterXfer.data           = g_master_txBuff;
    //DataSize cambia dependiendo de cuantos registros (segundos, minutos,etc) quieres modificar
    masterXfer.dataSize       = 8;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(EXAMPLE_I2C_MASTER_BASEADDR, &masterXfer);

    //Leer en el RTC
    /* subAddress = 0x01, data = g_master_rxBuff - read from slave.
      start + slaveaddress(w) + subAddress + repeated start + slaveaddress(r) + rx data buffer + stop */
    masterXfer.slaveAddress   = I2C_MASTER_SLAVE_ADDR_7BIT_RTC;
    masterXfer.direction      = kI2C_Read;
    masterXfer.subaddress     = (uint32_t)deviceAddress;
    masterXfer.subaddressSize = ONE;
    masterXfer.data           = g_master_rxBuff;
    masterXfer.dataSize       = I2C_DATA_LENGTH;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(EXAMPLE_I2C_MASTER_BASEADDR, &masterXfer);
}

int32_t write_RTC_hour(uint8_t hrs,uint8_t min,uint8_t seg)
{
	int32_t state = 0;
	g_master_txBuff[Sec] =  seg;
	g_master_txBuff[Min] =  min;
	g_master_txBuff[Hour] = hrs | BIT_12_MODE;

	g_master_txBuff[SQWE] = INITIAL_SQWE;


	i2c_master_transfer_t masterXfer;
    //Escribir en el RTC
    //Que slave quiero, dirección a 7 bits
    masterXfer.slaveAddress   = I2C_MASTER_SLAVE_ADDR_7BIT_RTC;
    //Si quiero leer o escribir
    masterXfer.direction      = kI2C_Write;
    //subaddress = Se refiere al registro interno al que queremos afectar
    //Si queremos segundos se pone 0, si queremos minutos 1, si queremos horas 2
    masterXfer.subaddress     = (uint32_t)deviceAddress;
    //Que tan grande es en bytes los valores que queremos afectar (memoria = 2, RTC = 1)
    masterXfer.subaddressSize = ONE;
    masterXfer.data           = g_master_txBuff;
    //DataSize cambia dependiendo de cuantos registros (segundos, minutos,etc) quieres modificar
    masterXfer.dataSize       = 8;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    state = I2C_MasterTransferBlocking(EXAMPLE_I2C_MASTER_BASEADDR, &masterXfer);

    return state;
}

int32_t write_RTC_date(uint8_t g_day,uint8_t g_month,uint8_t g_year)
{
	int32_t state = 0;
	g_master_txBuff[Day]  =  g_day;
	g_master_txBuff[Month]=  g_month;
	g_master_txBuff[Year] =  g_year;

	g_master_txBuff[SQWE] = INITIAL_SQWE;

	i2c_master_transfer_t masterXfer;
    //Escribir en el RTC
    //Que slave quiero, dirección a 7 bits
    masterXfer.slaveAddress   = I2C_MASTER_SLAVE_ADDR_7BIT_RTC;
    //Si quiero leer o escribir
    masterXfer.direction      = kI2C_Write;
    //subaddress = Se refiere al registro interno al que queremos afectar
    //Si queremos segundos se pone 0, si queremos minutos 1, si queremos horas 2
    masterXfer.subaddress     = (uint32_t)deviceAddress;
    //Que tan grande es en bytes los valores que queremos afectar (memoria = 2, RTC = 1)
    masterXfer.subaddressSize = ONE;
    masterXfer.data           = g_master_txBuff;
    //DataSize cambia dependiendo de cuantos registros (segundos, minutos,etc) quieres modificar
    masterXfer.dataSize       = 8;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(EXAMPLE_I2C_MASTER_BASEADDR, &masterXfer);

    //Leer en el RTC
    /* subAddress = 0x01, data = g_master_rxBuff - read from slave.
      start + slaveaddress(w) + subAddress + repeated start + slaveaddress(r) + rx data buffer + stop */
    masterXfer.slaveAddress   = I2C_MASTER_SLAVE_ADDR_7BIT_RTC;
    masterXfer.direction      = kI2C_Read;
    masterXfer.subaddress     = (uint32_t)deviceAddress;
    masterXfer.subaddressSize = ONE;
    masterXfer.data           = g_master_rxBuff;
    masterXfer.dataSize       = I2C_DATA_LENGTH;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    state = I2C_MasterTransferBlocking(EXAMPLE_I2C_MASTER_BASEADDR, &masterXfer);

    received_data_time.day    = g_master_rxBuff[Day];
    received_data_time.month  = g_master_rxBuff[Month];
    received_data_time.year   = g_master_rxBuff[Year];

    return state;
}

uint8_t get_RTC_hr_values(uint8_t place)
{
	return g_master_rxBuff[place];
}

void RTC_read_device(void)
{

	i2c_master_transfer_t masterXfer;

    /* subAddress = 0x01, data = g_master_rxBuff - read from slave.
      start + slaveaddress(w) + subAddress + repeated start + slaveaddress(r) + rx data buffer + stop */
    masterXfer.slaveAddress   = I2C_MASTER_SLAVE_ADDR_7BIT_RTC;
    masterXfer.direction      = kI2C_Read;
    masterXfer.subaddress     = (uint32_t)deviceAddress;
    masterXfer.subaddressSize = ONE;
    masterXfer.data           = g_master_rxBuff;
    masterXfer.dataSize       = I2C_DATA_LENGTH;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(EXAMPLE_I2C_MASTER_BASEADDR, &masterXfer);

    received_data_time.seconds = g_master_rxBuff[Sec];
    received_data_time.minutes = g_master_rxBuff[Min];
    received_data_time.hours   = g_master_rxBuff[Hour];

    received_data_time.day   = g_master_rxBuff[Day];
    received_data_time.month = g_master_rxBuff[Month];
    received_data_time.year  = g_master_rxBuff[Year];

    update_hr_time(received_data_time.hours, received_data_time.minutes,received_data_time.seconds);

    update_date(received_data_time.day, received_data_time.month,received_data_time.year);

    if(g_READ_HR_state == TRUE)
    {
    	if(g_UART_state == UART_0)
    	{
    		display_new_hr_UART0();
    	}

    	if(g_UART_state == UART_4)
    	{
    		display_new_hr_UART4();
    	}
    }
}

struct number_time RTC_read_device_log(void)
{
	i2c_master_transfer_t masterXfer_log;

    /* subAddress = 0x01, data = g_master_rxBuff - read from slave.
      start + slaveaddress(w) + subAddress + repeated start + slaveaddress(r) + rx data buffer + stop */
	masterXfer_log.slaveAddress   = I2C_MASTER_SLAVE_ADDR_7BIT_RTC;
	masterXfer_log.direction      = kI2C_Read;
	masterXfer_log.subaddress     = (uint32_t)deviceAddress;
    masterXfer_log.subaddressSize = ONE;
    masterXfer_log.data           = g_master_rxBuff_log;
    masterXfer_log.dataSize       = I2C_DATA_LENGTH;
    masterXfer_log.flags          = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(EXAMPLE_I2C_MASTER_BASEADDR, &masterXfer_log);

	//Get the Teen of Hours value on the low part
    received_data_time_log.teen_hours =  ((g_master_rxBuff_log[Hour] >> bit_4) & BITMASK_1BIT) + ASCII_OFFSET;
	//Get the Hours value on the low part
    received_data_time_log.hours =  (g_master_rxBuff_log[Hour] & BITMASK_4BIT) + ASCII_OFFSET;
	//Get the Teen of Minutes value on the high part
    received_data_time_log.teen_minutes =  ((g_master_rxBuff_log[Min] >> bit_4) & BITMASK_4BIT) + ASCII_OFFSET;//
	//Get the Minutes value on the low part
    received_data_time_log.minutes =  (g_master_rxBuff_log[Min] & BITMASK_4BIT) + ASCII_OFFSET;//
	//Get the Teen of Seconds value on the high part
    received_data_time_log.teen_seconds =  ((g_master_rxBuff_log[Sec] >> bit_4) & BITMASK_4BIT) + ASCII_OFFSET;//
	//Get the Seconds value on the low part
    received_data_time_log.seconds =  (g_master_rxBuff_log[Sec] & BITMASK_4BIT) + ASCII_OFFSET;//

	//Get the Teen of day value on the low part
    received_data_time_log.teen_day =  ((g_master_rxBuff_log[Day] >> bit_4) & BITMASK_4BIT) + ASCII_OFFSET;
	//Get the day value on the low part
    received_data_time_log.day =  (g_master_rxBuff_log[Day] & BITMASK_4BIT) + ASCII_OFFSET;
	//Get the Teen of month value on the low part
    received_data_time_log.teen_month =  ((g_master_rxBuff_log[Month] >> bit_4) & BITMASK_4BIT) + ASCII_OFFSET;
	//Get the month value on the low part
    received_data_time_log.month =  (g_master_rxBuff_log[Month] & BITMASK_4BIT) + ASCII_OFFSET;
	//Get the Teen of years value on the low part
    received_data_time_log.teen_year =  ((g_master_rxBuff_log[Year] >> bit_4) & BITMASK_4BIT) + ASCII_OFFSET;
	//Get the year value on the low part
    received_data_time_log.year =  (g_master_rxBuff_log[Year] & BITMASK_4BIT) + ASCII_OFFSET;


    return received_data_time_log;
}

void Read_Hr_State_flag (UART_Type *UART)
{
	if (UART0 == UART)
	{
		g_READ_HR_state = TRUE;
		g_UART_state = UART_0;
	}

	if(UART4 == UART)
	{
		g_READ_HR_state = TRUE;
		g_UART_state = UART_4;
	}
}

void reset_Read_Hr_State_flag (UART_Type *UART)
{
	if (UART0 == UART)
	{
		g_READ_HR_state = FALSE;
		g_UART_state = INICIALIZATION_VALUE;
	}

	if(UART4 == UART)
	{
		g_READ_HR_state = FALSE;
		g_UART_state = INICIALIZATION_VALUE;
	}
}
