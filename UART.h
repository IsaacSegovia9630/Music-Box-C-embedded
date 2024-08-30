/*
 * UART.h
 *
 *  Created on: 11 oct 2022
 *      Author: Tahirí Ramos
 */
/********************************************************************************************************
Set Display Attributes
Set Attribute Mode	<ESC>[{attr1};...;{attrn}m
Sets multiple display attribute settings. The following lists standard attributes:
0	Reset all attributes
1	Bright
2	Dim
4	Underscore
5	Blink
7	Reverse
8	Hidden

	Foreground Colors
30	Black
31	Red
32	Green
33	Yellow
34	Blue
35	Magenta
36	Cyan
37	White

	Background Colors
40	Black
41	Red
42	Green
43	Yellow
44	Blue
45	Magenta
46	Cyan
47	White

SOURCE:
http://graphcomp.com/info/specs/ansi_col.html
*********************************************************************************************************/

#ifndef EQUIPO_6_T7_UART_H_
#define EQUIPO_6_T7_UART_H_

#include "stdint.h"
#include "MK64F12.h"



#define UART_CLK_FREQ   CLOCK_GetFreq(UART0_CLK_SRC)
#define UART_BAUDRATE (9600)
#define UART_DATA_LENGTH  (1)
#define DELAY_SWEEP (6.5F)

#define ASCII_OFFSET '0'
#define BITMASK_4BIT 0xf
#define BITMASK_1BIT 0x1

#define HOURS_TENS 0
#define HOURS 1
#define MINUTES_TENS 2
#define MINUTES 3
#define SECONDS_TENS 4
#define SECONDS 5

#define ESC			(0x1B)
#define ENTER		(0xD)
#define BOTON_1		(0x31)
#define BOTON_2		(0x32)
#define BOTON_3		(0x33)
#define BOTON_4		(0x34)
#define BOTON_5	    (0x35)
#define BOTON_6	    (0x36)
#define BOTON_7	    (0x37)
#define BOTON_8	    (0x38)
#define No	        (0x6E)
#define Yes	        (0x79)


#define CLEAR_SREEN		8
#define CURSOR_POS_XY	12
#define INITIAL 		14
#define TEXT_1			14
#define TEXT_2			20
#define TEXT_3			23
#define TEXT_4			8
#define TEXT_5			47
#define SPACE			3

#define ZERO    0
#define ONE 	1
#define TWO		2
#define THREE	3
#define FOUR	4
#define FIVE	5
#define SIX		6
#define SEVEN	7
#define EIGHT	8
#define NINE	9

#define NUM_HEX_30 	0x30
#define NUM_HEX_F	0xF
#define NUM_HEX_7	0x7

#define ARRAY_OFFSET 6U

#define NULL_CHARACTER 1

#define DOUBLE_POINT_POS1 2

#define DOUBLE_POINT_POS2 5

#define NULL_CHARACTER_POS 8

#define RECEIVED_DATA_BUFFER_SIZE 2

#define DATA_FINAL_TEMP_length 19

#define DATA_FINAL_HUM_length 11

#define DATA_FINAL_HRS_length 13

#define DATA_FINAL_DATE_length 10

typedef enum{UART_0, /*!< Definition to select UART0*/
			 UART_1, /*!< Definition to select UART1 */
			 UART_2, /*!< Definition to select UART2 */
			 UART_3, /*!< Definition to select UART3 */
			 UART_4, /*!< Definition to select UART4 */
			 UART_5, /*!< Definition to select UART5 */
			} UART_name_t;


/*Aqui configuramos e inicializamos los pines que vamos a necesitar, ademas de las interrupciones*/
void init_UART (void);

/*las siguientes 3 funcines son para configurar lo que nos va a mostrar el display de cada opcion*/
void display_menu(UART_Type *UART);

void display_read_hr(UART_Type *UART);

void display_config_hr(UART_Type *UART);

void display_config_date(UART_Type *UART);

void display_date(UART_Type *UART);

void display_read_temp(UART_Type *UART);

void display_read_hum(UART_Type *UART);

void display_active_log(UART_Type *UART);

void display_active_log_time(UART_Type *UART);

void display_active_log_capture(UART_Type *UART);

void display_read_log(UART_Type *UART);

void display_read_log_from_memory(UART_Type *UART);

void display_new_hr_UART0(void);

void display_new_hr_UART4(void);

void update_hr_time(uint8_t hours, uint8_t minutes, uint8_t seconds);

void update_date(uint8_t day, uint8_t month, uint8_t year);

void update_hum(uint8_t hum);

void update_temp(uint8_t temp_tens, uint8_t temp_units, uint8_t temp_first_decimal, uint8_t temp_second_decimal);

void update_data_final(uint8_t temp_tens, uint8_t temp_units, uint8_t temp_first_decimal, uint8_t temp_second_decimal, uint8_t hum, uint8_t teen_hours, uint8_t hours, uint8_t teen_minutes, uint8_t minutes, uint8_t teen_seconds, uint8_t seconds, uint8_t teen_day, uint8_t day, uint8_t teen_month, uint8_t month, uint8_t teen_year, uint8_t year);

void update_data_final2(uint8_t temp_tens, uint8_t temp_units, uint8_t temp_first_decimal, uint8_t temp_second_decimal, uint8_t hum, uint8_t teen_hours, uint8_t hours, uint8_t teen_minutes, uint8_t minutes, uint8_t teen_seconds, uint8_t seconds, uint8_t teen_day, uint8_t day, uint8_t teen_month, uint8_t month, uint8_t teen_year, uint8_t year);

void clear_screen(UART_Type *UART);

uint8_t get_data_length(uint8_t *array);

void reset_terminal(void);

void change_display_attributes_terminal(void);

void resource_not_available(UART_Type *UART);

void write_on_screen(UART_Type *UART);

uint8_t get_data(uint8_t lugar, UART_Type *UART);

uint8_t get_data_log(uint8_t lugar, UART_Type *UART);

uint8_t get_UART_value(UART_Type *UART);

uint8_t get_UART_value_direct(UART_Type *UART);

void set_UART_flag_false(UART_Type *UART);

uint8_t get_UART0_flag(void);

uint8_t get_UART4_flag(void);

void reset_UART_buffer_values(UART_Type *UART);

void reset_UART_buffer_values_log(UART_Type *UART);

void I2C_device_fail_communication(UART_Type *UART);

void confirmation_CONFIG_HR(UART_Type *UART);

void confirmation_CONFIG_DATE(UART_Type *UART);

void confirmation_ACTIVATION_LOG(UART_Type *UART);

void reading_log_repositioning(UART_Type *UART);

void move_down_cursor(UART_Type *UART);

#endif /* EQUIPO_6_T7_UART_H_ */
