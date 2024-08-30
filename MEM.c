/*
 * MEM.c
 *
 *  Created on: 26/10/2022
 *      Author: Tahirí Ramos
 */
#include "MEM.h"
#include "bits.h"
#include "UART.h"
#include "RTC.h"
#include "fsl_gpio.h"
#include "fsl_port.h"


uint8_t Rx_buff_mem[BUFF_MEM];
uint8_t Tx_buff_mem[BUFF_MEM];

struct data
{
    uint8_t temp_tens;
    uint8_t temp_units;
    uint8_t temp_first_decimal;
    uint8_t temp_second_decimal;
    uint8_t hum;
    uint8_t teen_seconds;
    uint8_t seconds;
    uint8_t teen_minutes;
    uint8_t minutes;
    uint8_t teen_hours;
    uint8_t hours;
    uint8_t teen_day;
    uint8_t day;
    uint8_t teen_month;
    uint8_t month;
    uint8_t teen_year;
    uint8_t year;
}data_readed_digits;



void write_mem(uint8_t MSJ[], uint16_t mem_adress, uint16_t size)
{

	i2c_master_transfer_t masterXfer_mem;
	for(uint8_t i = 0; i < size; i++)
	{
		Tx_buff_mem[i] = MSJ[i];
	}

    //Que slave quiero, dirección a 7 bits
	masterXfer_mem.slaveAddress   = MEM_MASTER_SLAVE_ADDR_7BIT;
    //Si quiero leer o escribir
	masterXfer_mem.direction      = kI2C_Write;
    //subaddress = Se refiere al registro interno al que queremos afectar
	masterXfer_mem.subaddress     = mem_adress;
    //Que tan grande es en bytes los valores que queremos afectar (memoria = 2, RTC = 1)
	masterXfer_mem.subaddressSize = MEM_ADRESS_LENGTH;
	masterXfer_mem.data           = Tx_buff_mem;
    //DataSize cambia dependiendo de cuantos registros quieres modificar
	masterXfer_mem.dataSize       = size;
	masterXfer_mem.flags          = kI2C_TransferDefaultFlag;

	I2C_MasterTransferBlocking(I2C_MEM_MASTER_BASEADDR, &masterXfer_mem);

}


void read_mem(uint16_t mem_adress, uint16_t size, UART_Type *UART)
{

	i2c_master_transfer_t masterXfer_mem2;

	masterXfer_mem2.slaveAddress   = MEM_MASTER_SLAVE_ADDR_7BIT;
	masterXfer_mem2.direction      = kI2C_Read;
	masterXfer_mem2.subaddress     = mem_adress;
	masterXfer_mem2.subaddressSize = MEM_ADRESS_LENGTH;
	masterXfer_mem2.data           = Rx_buff_mem;
	masterXfer_mem2.dataSize       = size;
	masterXfer_mem2.flags          = kI2C_TransferDefaultFlag;

	I2C_MasterTransferBlocking(I2C_MEM_MASTER_BASEADDR, &masterXfer_mem2);

	data_readed_digits.temp_tens = Rx_buff_mem[0];
	data_readed_digits.temp_units = Rx_buff_mem[1];
	data_readed_digits.temp_first_decimal = Rx_buff_mem[2];
	data_readed_digits.temp_second_decimal = Rx_buff_mem[3];
	data_readed_digits.hum = Rx_buff_mem[4];

	data_readed_digits.teen_hours = Rx_buff_mem[5];
	data_readed_digits.hours = Rx_buff_mem[6];
	data_readed_digits.teen_minutes = Rx_buff_mem[7];
	data_readed_digits.minutes = Rx_buff_mem[8];
	data_readed_digits.teen_seconds = Rx_buff_mem[9];
	data_readed_digits.seconds = Rx_buff_mem[10];
	data_readed_digits.teen_day = Rx_buff_mem[11];
	data_readed_digits.day = Rx_buff_mem[12];
	data_readed_digits.teen_month = Rx_buff_mem[13];
	data_readed_digits.month = Rx_buff_mem[14];
	data_readed_digits.teen_year = Rx_buff_mem[15];
	data_readed_digits.year = Rx_buff_mem[16];

	if(UART == UART0)
	{
		update_data_final(data_readed_digits.temp_tens, data_readed_digits.temp_units, data_readed_digits.temp_first_decimal, data_readed_digits.temp_second_decimal, data_readed_digits.hum, data_readed_digits.teen_hours, data_readed_digits.hours, data_readed_digits.teen_minutes, data_readed_digits.minutes, data_readed_digits.teen_seconds, data_readed_digits.seconds, data_readed_digits.teen_day, data_readed_digits.day, data_readed_digits.teen_month, data_readed_digits.month, data_readed_digits.teen_year, data_readed_digits.year);
	}
	if(UART == UART4)
	{
		update_data_final2(data_readed_digits.temp_tens, data_readed_digits.temp_units, data_readed_digits.temp_first_decimal, data_readed_digits.temp_second_decimal, data_readed_digits.hum, data_readed_digits.teen_hours, data_readed_digits.hours, data_readed_digits.teen_minutes, data_readed_digits.minutes, data_readed_digits.teen_seconds, data_readed_digits.seconds, data_readed_digits.teen_day, data_readed_digits.day, data_readed_digits.teen_month, data_readed_digits.month, data_readed_digits.teen_year, data_readed_digits.year);
	}

}

void write_mem_log_init(UART_Type *UART, uint8_t time, uint8_t capture)
{
	set_max_data_capture(UART, capture);
	pit_init(UART, time);

}

uint8_t mem_communication_test(void)
{
	uint8_t state;
	i2c_master_transfer_t masterXfer_mem;
    //Que slave quiero, dirección a 7 bits
	masterXfer_mem.slaveAddress   = MEM_MASTER_SLAVE_ADDR_7BIT;
    //Si quiero leer o escribir
	masterXfer_mem.direction      = kI2C_Write;
    //subaddress = Se refiere al registro interno al que queremos afectar
	masterXfer_mem.subaddress     = SUB_ADRESS_TEST;
    //Que tan grande es en bytes los valores que queremos afectar (memoria = 2, RTC = 1)
	masterXfer_mem.subaddressSize = MEM_ADRESS_LENGTH;
	masterXfer_mem.data           = Tx_buff_mem;
    //DataSize cambia dependiendo de cuantos registros quieres modificar
	masterXfer_mem.dataSize       = BUFF_MEM;
	masterXfer_mem.flags          = kI2C_TransferDefaultFlag;

	state = I2C_MasterTransferBlocking(I2C_MEM_MASTER_BASEADDR, &masterXfer_mem);

	return state;
}
