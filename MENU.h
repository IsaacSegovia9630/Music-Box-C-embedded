/*
 * MENU.h
 *
 *  Created on: 18 oct 2022
 *      Author: isaac
 */

#ifndef MENU_H_
#define MENU_H_
#include "MK64F12.h"

#define PLACE0 0
#define PLACE1 1

#define SUCCES 1105

#define g_capture_data_length 17

#define INICIALIZATION_VALUE 6

enum MODES
{
	Esc,
	Read_HR,
	Config_HR,
	Read_date,
	Congif_date,
	Read_temperature,
	Read_Humidity,
	Activate_Log,
	Read_Log,
	YES,
	NO,
	AnythingElse
};

typedef struct
{
	uint8_t dig_1;
	uint8_t dig_0;
}buffer_t;

uint8_t Get_Menu_input(UART_Type *UART);

uint8_t Get_Log_input(UART_Type *UART);

void select_menu(UART_Type *UART, uint8_t uart);

void read_hr(UART_Type *UART);

void config_hr(UART_Type *UART);

void config_date(UART_Type *UART);

void read_date(UART_Type *UART);

void read_temp(UART_Type *UART);

void read_hum(UART_Type *UART);

void activate_log(UART_Type *UART);

void read_log(UART_Type *UART);

void read_log_from_memory1(UART_Type *UART);

#endif /* MENU_H_ */
