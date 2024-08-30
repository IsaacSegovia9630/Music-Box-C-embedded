/*
 * MENU.c
 *
 *  Created on: 18 oct 2022
 *      Author: isaac
 */

#include "MENU.h"
#include "UART.h"
#include "RTC.h"
#include "bits.h"
#include "SENSOR_TEMP_HUM.h"
#include "MEM.h"
#include "fsl_common.h"

static uint8_t g_resource_READ_HR          = FALSE;
static uint8_t g_resource_CONFIG_HR        = FALSE;
static uint8_t g_resource_READ_DATE        = FALSE;
static uint8_t g_resource_CONFIG_DATE      = FALSE;
static uint8_t g_resource_READ_TEMPERATURE = FALSE;
static uint8_t g_resource_READ_HUMIDITY    = FALSE;
static uint8_t g_resource_ACTIVATE_LOG1    = FALSE;
static uint8_t g_resource_ACTIVATE_LOG2    = FALSE;
static uint8_t g_resource_READ_LOG1        = FALSE;
static uint8_t g_resource_READ_LOG2        = FALSE;

static uint8_t g_UART_using_resource_READ_HR          = INICIALIZATION_VALUE;
static uint8_t g_UART_using_resource_CONFIG_HR        = INICIALIZATION_VALUE;
static uint8_t g_UART_using_resource_READ_DATE        = INICIALIZATION_VALUE;
static uint8_t g_UART_using_resource_CONFIG_DATE      = INICIALIZATION_VALUE;
static uint8_t g_UART_using_resource_READ_TEMPERATURE = INICIALIZATION_VALUE;
static uint8_t g_UART_using_resource_READ_HUMIDITY    = INICIALIZATION_VALUE;
static uint8_t g_UART_using_resource_ACTIVATE_LOG1    = INICIALIZATION_VALUE;
static uint8_t g_UART_using_resource_ACTIVATE_LOG2    = INICIALIZATION_VALUE;
static uint8_t g_UART_using_resource_READ_LOG1    = INICIALIZATION_VALUE;
static uint8_t g_UART_using_resource_READ_LOG2    = INICIALIZATION_VALUE;

static uint8_t g_start_log_capture = FALSE;
static uint8_t g_start_log_capture2 = FALSE;

static uint8_t g_start_log_reading = FALSE;
static uint8_t g_start_log_reading2 = FALSE;


static uint16_t mem_adress_access_initial_value  = 0x0;
static uint16_t mem_adress_access_initial_value2 = 0x5DF0;

buffer_t g_Config_hr;

buffer_t g_Config_date;

buffer_t g_Activate_log1;

buffer_t g_Activate_log2;

static uint8_t g_hrs = 0;
static uint8_t g_min = 0;
static uint8_t g_seg = 0;

static uint8_t g_day   = 0;
static uint8_t g_month = 0;
static uint8_t g_year  = 0;

static uint8_t g_time1    = 0;
static uint8_t g_capture1 = 0;
static uint8_t g_time2    = 0;
static uint8_t g_capture2 = 0;

static uint8_t g_counter_CONFIG_HOUR = ZERO;

static uint8_t g_counter_CONFIG_HOUR_CONFIG_DATE = ZERO;

static uint8_t g_counter_ACTIVATE_LOG1 = ZERO;

static uint8_t g_counter_ACTIVATE_LOG2 = ZERO;

void select_menu(UART_Type *UART, uint8_t uart)
{
	if(TRUE == g_resource_ACTIVATE_LOG1 && UART_0 == uart)
	{
		uint8_t status = Get_Log_input(UART);

		if(NO == status)
		{
			g_resource_ACTIVATE_LOG1 = FALSE;
			g_counter_ACTIVATE_LOG1 = ZERO;
			reset_UART_buffer_values(UART);
			set_UART_flag_false(UART);
			display_menu(UART);
			return;
		}
		if(YES == status)
		{
			g_start_log_capture = TRUE;
			reset_UART_buffer_values_log(UART);
			display_active_log_time(UART);
		}

		if(g_start_log_capture == TRUE)
		{
			if(ONE == g_counter_ACTIVATE_LOG1)
			{
				g_Activate_log1.dig_0 = get_data_log(PLACE0, UART) - ASCII_OFFSET; //Tiempo de captura
				reset_UART_buffer_values_log(UART);

			}
			if(TWO == g_counter_ACTIVATE_LOG1)
			{
				g_Activate_log1.dig_1 = (get_data_log(PLACE0, UART) - ASCII_OFFSET) & BITMASK_4BIT; //Tiempo de captura
				g_time1 = (g_Activate_log1.dig_0 << bit_4) | g_Activate_log1.dig_1;
				reset_UART_buffer_values_log(UART);

			}
			if(THREE == g_counter_ACTIVATE_LOG1)
			{
				display_active_log_capture(UART);
				reset_UART_buffer_values_log(UART);

			}
			if(FOUR == g_counter_ACTIVATE_LOG1)
			{
				g_Activate_log1.dig_0 = get_data_log(PLACE0, UART) - ASCII_OFFSET; //Cantidad de capturas
				reset_UART_buffer_values_log(UART);
			}
			if(FIVE == g_counter_ACTIVATE_LOG1)
			{
				g_Activate_log1.dig_1 = (get_data_log(PLACE0, UART) - ASCII_OFFSET) & BITMASK_4BIT; //Tiempo de captura
				g_capture1 = (g_Activate_log1.dig_0 << bit_4) | g_Activate_log1.dig_1;
				reset_UART_buffer_values_log(UART);

			}
			else if((SIX <= g_counter_ACTIVATE_LOG1) && (ENTER == get_data_log(PLACE0, UART)))
			{

				if(kStatus_Success == mem_communication_test())
				{
					confirmation_ACTIVATION_LOG(UART);
					write_mem_log_init(UART, g_time1, g_capture1);
					g_counter_ACTIVATE_LOG1 = ZERO;
					g_start_log_capture = FALSE;
					reset_UART_buffer_values_log(UART);
					set_UART_flag_false(UART);
					return;
				}
				else
				{
					I2C_device_fail_communication(UART);
					g_counter_ACTIVATE_LOG1 = ZERO;
					g_start_log_capture = FALSE;
					reset_UART_buffer_values_log(UART);
					set_UART_flag_false(UART);
					return;
				}
			}

			g_counter_ACTIVATE_LOG1++;
		}

		uint8_t Menu_option;

		Menu_option = Get_Menu_input(UART);

		if(Esc == Menu_option)
		{
			if( (TRUE == g_resource_ACTIVATE_LOG1) && (uart == g_UART_using_resource_ACTIVATE_LOG1) )
			{
				display_menu(UART);
				reset_UART_buffer_values_log(UART);
				set_UART_flag_false(UART);
				g_resource_ACTIVATE_LOG1 = FALSE;
				g_UART_using_resource_ACTIVATE_LOG1 = INICIALIZATION_VALUE;
				return;
			}
		}

		set_UART_flag_false(UART);

		return;
	}

	if(TRUE == g_resource_ACTIVATE_LOG2 && UART_4 == uart)
	{
		uint8_t status = Get_Log_input(UART);

		if(NO == status)
		{
			g_resource_ACTIVATE_LOG2 = FALSE;
			g_counter_ACTIVATE_LOG2 = ZERO;
			reset_UART_buffer_values(UART);
			set_UART_flag_false(UART);
			display_menu(UART);
			return;
		}
		if(YES == status)
		{
			g_start_log_capture2 = TRUE;
			reset_UART_buffer_values_log(UART);
			display_active_log_time(UART);
		}

		if(g_start_log_capture2 == TRUE)
		{
			if(ONE == g_counter_ACTIVATE_LOG2)
			{
				g_Activate_log2.dig_0 = get_data_log(PLACE0, UART) - ASCII_OFFSET; //Tiempo de captura
				reset_UART_buffer_values_log(UART);

			}
			if(TWO == g_counter_ACTIVATE_LOG2)
			{
				g_Activate_log2.dig_1 = (get_data_log(PLACE0, UART) - ASCII_OFFSET) & BITMASK_4BIT; //Tiempo de captura
				g_time2 = (g_Activate_log2.dig_0 << bit_4) | g_Activate_log2.dig_1;
				reset_UART_buffer_values_log(UART);

			}
			if(THREE == g_counter_ACTIVATE_LOG2)
			{
				display_active_log_capture(UART);
				reset_UART_buffer_values_log(UART);

			}
			if(FOUR == g_counter_ACTIVATE_LOG2)
			{
				g_Activate_log2.dig_0 = get_data_log(PLACE0, UART) - ASCII_OFFSET; //Cantidad de capturas
				reset_UART_buffer_values_log(UART);
			}
			if(FIVE == g_counter_ACTIVATE_LOG2)
			{
				g_Activate_log2.dig_1 = (get_data_log(PLACE0, UART) - ASCII_OFFSET) & BITMASK_4BIT; //Tiempo de captura
				g_capture2 = (g_Activate_log2.dig_0 << bit_4) | g_Activate_log2.dig_1;
				reset_UART_buffer_values_log(UART);

			}
			else if((SIX <= g_counter_ACTIVATE_LOG2) && (ENTER == get_data_log(PLACE0, UART)))
			{

				if(kStatus_Success == mem_communication_test())
				{
					confirmation_ACTIVATION_LOG(UART);
					write_mem_log_init(UART, g_time2, g_capture2);
					g_counter_ACTIVATE_LOG2 = ZERO;
					g_start_log_capture2 = FALSE;
					reset_UART_buffer_values_log(UART);
					set_UART_flag_false(UART);
					return;
				}
				else
				{
					I2C_device_fail_communication(UART);
					g_counter_ACTIVATE_LOG2 = ZERO;
					g_start_log_capture2 = FALSE;
					reset_UART_buffer_values_log(UART);
					set_UART_flag_false(UART);
					return;
				}
			}

			g_counter_ACTIVATE_LOG2++;
		}

		uint8_t Menu_option;

		Menu_option = Get_Menu_input(UART);

		if(Esc == Menu_option)
		{
			if( (TRUE == g_resource_ACTIVATE_LOG2) && (uart == g_UART_using_resource_ACTIVATE_LOG2) )
			{
				display_menu(UART);
				reset_UART_buffer_values_log(UART);
				set_UART_flag_false(UART);
				g_resource_ACTIVATE_LOG2 = FALSE;
				g_UART_using_resource_ACTIVATE_LOG2 = INICIALIZATION_VALUE;
				return;
			}
		}

		set_UART_flag_false(UART);

		return;
	}

	if(TRUE == g_resource_READ_LOG1 && UART_0 == uart)
	{
		uint8_t status = Get_Log_input(UART);

		if(NO == status)
		{
			g_resource_READ_LOG1 = FALSE;
			g_UART_using_resource_READ_LOG1 = ZERO;
			reset_UART_buffer_values(UART);
			set_UART_flag_false(UART);
			display_menu(UART);
			return;
		}
		if(YES == status)
		{
			g_start_log_reading = TRUE;
			reset_UART_buffer_values_log(UART);
		}

		if(g_start_log_reading == TRUE)
		{
			read_log_from_memory1(UART);
		}

		uint8_t Menu_option;

		Menu_option = Get_Menu_input(UART);

		if(Esc == Menu_option)
		{
			if( (TRUE == g_resource_READ_LOG1) && (uart == g_UART_using_resource_READ_LOG1) )
			{
				display_menu(UART);
				g_resource_READ_LOG1 = FALSE;
				g_UART_using_resource_READ_LOG1 = INICIALIZATION_VALUE;
				return;
			}
		}

		set_UART_flag_false(UART);

		return;
	}

	if(TRUE == g_resource_READ_LOG2 && UART_0 == uart)
	{
		uint8_t status = Get_Log_input(UART);

		if(NO == status)
		{
			g_resource_READ_LOG2 = FALSE;
			g_UART_using_resource_READ_LOG2 = ZERO;
			reset_UART_buffer_values(UART);
			set_UART_flag_false(UART);
			display_menu(UART);
			return;
		}
		if(YES == status)
		{
			g_start_log_reading2 = TRUE;
			reset_UART_buffer_values_log(UART);
		}

		if(g_start_log_reading2 == TRUE)
		{
			read_log_from_memory1(UART);
		}

		uint8_t Menu_option;

		Menu_option = Get_Menu_input(UART);

		if(Esc == Menu_option)
		{
			if( (TRUE == g_resource_READ_LOG2) && (uart == g_UART_using_resource_READ_LOG2) )
			{
				display_menu(UART);
				g_resource_READ_LOG2 = FALSE;
				g_UART_using_resource_READ_LOG2 = INICIALIZATION_VALUE;
				return;
			}
		}

		set_UART_flag_false(UART);

		return;
	}

	uint8_t Menu_option;

	Menu_option = Get_Menu_input(UART);

	switch(Menu_option)
	{
		case Esc:

			if( (TRUE == g_resource_READ_HR) && (uart == g_UART_using_resource_READ_HR) )
			{
				display_menu(UART);
				g_resource_READ_HR = FALSE;
				g_UART_using_resource_READ_HR = INICIALIZATION_VALUE;
				reset_Read_Hr_State_flag (UART);
				return;
			}

			if( (TRUE == g_resource_CONFIG_HR) && (uart == g_UART_using_resource_CONFIG_HR) )
			{
				display_menu(UART);
				g_resource_CONFIG_HR = FALSE;
				g_UART_using_resource_CONFIG_HR = INICIALIZATION_VALUE;

				return;
			}

			if( (TRUE == g_resource_READ_DATE) && (uart == g_UART_using_resource_READ_DATE) )
			{
				display_menu(UART);
				g_resource_READ_DATE = FALSE;
				g_UART_using_resource_READ_DATE = INICIALIZATION_VALUE;
				return;
			}

			if( (TRUE == g_resource_CONFIG_DATE) && (uart == g_UART_using_resource_CONFIG_DATE) )
			{
				display_menu(UART);
				g_resource_CONFIG_DATE = FALSE;
				g_UART_using_resource_CONFIG_DATE = INICIALIZATION_VALUE;

				return;
			}

			if( (TRUE == g_resource_READ_TEMPERATURE) && (uart == g_UART_using_resource_READ_TEMPERATURE) )
			{
				display_menu(UART);
				g_resource_READ_TEMPERATURE = FALSE;
				g_UART_using_resource_READ_TEMPERATURE = INICIALIZATION_VALUE;
				return;
			}

			if( (TRUE == g_resource_READ_HUMIDITY) && (uart == g_UART_using_resource_READ_HUMIDITY) )
			{
				display_menu(UART);
				g_resource_READ_HUMIDITY = FALSE;
				g_UART_using_resource_READ_HUMIDITY = INICIALIZATION_VALUE;
				return;
			}

			display_menu(UART);

		break;

		case Read_HR:

			if(FALSE == g_resource_READ_HR)
			{
				g_resource_READ_HR = TRUE;
				if (UART0 == UART)
				{
					g_UART_using_resource_READ_HR = UART_0;
					Read_Hr_State_flag (UART);
				}

				if(UART4 == UART)
				{
					g_UART_using_resource_READ_HR = UART_4;
					Read_Hr_State_flag (UART);
				}
				read_hr(UART);
			}
			else
			{
				resource_not_available(UART);
			}

		break;

		case Config_HR:

			if(FALSE == g_resource_CONFIG_HR)
			{
				g_resource_CONFIG_HR = TRUE;

				if (UART0 == UART)
				{
					g_UART_using_resource_CONFIG_HR = UART_0;
				}

				if(UART4 == UART)
				{
					g_UART_using_resource_CONFIG_HR = UART_4;
				}
				config_hr(UART);
			}
			else
			{
				resource_not_available(UART);
			}

		break;

		case Read_date:

			if(FALSE == g_resource_READ_DATE)
			{
				g_resource_READ_DATE = TRUE;

				if (UART0 == UART)
				{
					g_UART_using_resource_READ_DATE = UART_0;
				}

				if(UART4 == UART)
				{
					g_UART_using_resource_READ_DATE = UART_4;
				}
				read_date(UART);
			}
			else
			{
				resource_not_available(UART);
			}

		break;

		case Congif_date:

			if(FALSE == g_resource_CONFIG_DATE)
			{
				g_resource_CONFIG_DATE = TRUE;

				if (UART0 == UART)
				{
					g_UART_using_resource_CONFIG_DATE = UART_0;
				}

				if(UART4 == UART)
				{
					g_UART_using_resource_CONFIG_DATE = UART_4;
				}
				config_date(UART);
			}
			else
			{
				resource_not_available(UART);
			}

		break;

		case Read_temperature:

			if(FALSE == g_resource_READ_TEMPERATURE)
			{
				g_resource_READ_TEMPERATURE = TRUE;

				if (UART0 == UART)
				{
					g_UART_using_resource_READ_TEMPERATURE = UART_0;
				}

				if(UART4 == UART)
				{
					g_UART_using_resource_READ_TEMPERATURE = UART_4;
				}
				read_temp(UART);
			}
			else
			{
				resource_not_available(UART);
			}

		break;

		case Read_Humidity:

			if(FALSE == g_resource_READ_HUMIDITY)
			{
				g_resource_READ_HUMIDITY = TRUE;

				if (UART0 == UART)
				{
					g_UART_using_resource_READ_HUMIDITY = UART_0;
				}

				if(UART4 == UART)
				{
					g_UART_using_resource_READ_HUMIDITY = UART_4;
				}
				read_hum(UART);
			}
			else
			{
				resource_not_available(UART);
			}

		break;

		case Activate_Log:
			if(FALSE == g_resource_ACTIVATE_LOG1 && UART == UART0)
			{
				g_resource_ACTIVATE_LOG1 = TRUE;
				g_UART_using_resource_ACTIVATE_LOG1 = UART_0;
				activate_log(UART);

			}

			if(FALSE == g_resource_ACTIVATE_LOG2 && UART == UART4)
			{
				g_resource_ACTIVATE_LOG2 = TRUE;
				g_UART_using_resource_ACTIVATE_LOG2 = UART_4;
				activate_log(UART);
			}

		break;

		case Read_Log:
			if(FALSE == g_resource_READ_LOG1 && UART == UART0)
			{
				g_resource_READ_LOG1 = TRUE;
				g_UART_using_resource_READ_LOG1 = UART_0;
				read_log(UART);

			}

			if(FALSE == g_resource_READ_LOG2 && UART == UART4)
			{
				g_resource_READ_LOG2 = TRUE;
				g_UART_using_resource_READ_LOG2 = UART_4;
				read_log(UART);
			}

		break;

		case AnythingElse:

		break;

	}

	//The next code, capture the data to configure the Hour on the RTC device
	if(uart == g_UART_using_resource_CONFIG_HR)
	{
		if(ONE == g_counter_CONFIG_HOUR) //Tens of Hours
		{
			g_Config_hr.dig_0 = get_data(PLACE0, UART) - ASCII_OFFSET;
		}
		else if(TWO == g_counter_CONFIG_HOUR)//Units of Hours
		{
			g_Config_hr.dig_1 = get_data(PLACE0, UART) & BITMASK_4BIT;
			g_hrs = (g_Config_hr.dig_0 << bit_4) | g_Config_hr.dig_1;
		}
		else if(THREE == g_counter_CONFIG_HOUR)//Tens of Minutes
		{
			g_Config_hr.dig_0 = get_data(PLACE0, UART) - ASCII_OFFSET;
		}
		else if(FOUR == g_counter_CONFIG_HOUR)//Units of Minutes
		{
			g_Config_hr.dig_1 = get_data(PLACE0, UART) & BITMASK_4BIT;
			g_min = (g_Config_hr.dig_0 << bit_4) | g_Config_hr.dig_1;
		}
		else if(FIVE == g_counter_CONFIG_HOUR)//Tens of Seconds
		{
			g_Config_hr.dig_0 = get_data(PLACE0, UART) - ASCII_OFFSET;
		}
		else if(SIX == g_counter_CONFIG_HOUR)//Units of Seconds
		{
			g_Config_hr.dig_1 = get_data(PLACE0, UART) & BITMASK_4BIT;
			g_seg = (g_Config_hr.dig_0 << bit_4) | g_Config_hr.dig_1;
		}
		else if( (SEVEN <= g_counter_CONFIG_HOUR) && (ENTER == get_data(PLACE0, UART)) )// Finish Configure Hour
		{
			if(write_RTC_hour(g_hrs,g_min,g_seg) == kStatus_Success)
			{
				confirmation_CONFIG_HR(UART);
			}
			else if(write_RTC_hour(g_hrs,g_min,g_seg) == kStatus_Fail)
			{
				I2C_device_fail_communication(UART);
			}

			g_counter_CONFIG_HOUR = ZERO;

			reset_UART_buffer_values(UART);

			set_UART_flag_false(UART);

			return;
		}

		g_counter_CONFIG_HOUR++;

		reset_UART_buffer_values(UART);

		set_UART_flag_false(UART);
	}

	//The next code, capture the data to configure the date on the RTC device
	if(uart == g_UART_using_resource_CONFIG_DATE)
	{
		if(ONE == g_counter_CONFIG_HOUR_CONFIG_DATE) //Tens of Days
		{
			g_Config_date.dig_0 = get_data(PLACE0, UART) - ASCII_OFFSET;
		}
		else if(TWO == g_counter_CONFIG_HOUR_CONFIG_DATE)//Units of Days
		{
			g_Config_date.dig_1 = get_data(PLACE0, UART) & BITMASK_4BIT;
			g_day = (g_Config_date.dig_0 << bit_4) | g_Config_date.dig_1;
		}
		else if(THREE == g_counter_CONFIG_HOUR_CONFIG_DATE)//Tens of Months
		{
			g_Config_date.dig_0 = get_data(PLACE0, UART) - ASCII_OFFSET;
		}
		else if(FOUR == g_counter_CONFIG_HOUR_CONFIG_DATE)//Units of Months
		{
			g_Config_date.dig_1 = get_data(PLACE0, UART) & BITMASK_4BIT;
			g_month = (g_Config_date.dig_0 << bit_4) | g_Config_date.dig_1;
		}
		else if(FIVE == g_counter_CONFIG_HOUR_CONFIG_DATE)//Tens of Years
		{
			g_Config_date.dig_0 = get_data(PLACE0, UART) - ASCII_OFFSET;
		}
		else if(SIX == g_counter_CONFIG_HOUR_CONFIG_DATE)//Units of Years
		{
			g_Config_date.dig_1 = get_data(PLACE0, UART) & BITMASK_4BIT;
			g_year = (g_Config_date.dig_0 << bit_4) | g_Config_date.dig_1;
		}
		else if( (SEVEN <= g_counter_CONFIG_HOUR_CONFIG_DATE) && (ENTER == get_data(PLACE0, UART)) )// Finish Configure Date
		{
			if(write_RTC_date(g_day,g_month,g_year) == kStatus_Success)
			{
				confirmation_CONFIG_DATE(UART);
			}
			else if(write_RTC_date(g_day,g_month,g_year) == kStatus_Fail)
			{
				I2C_device_fail_communication(UART);
			}

			g_counter_CONFIG_HOUR_CONFIG_DATE = ZERO;

			reset_UART_buffer_values(UART);

			set_UART_flag_false(UART);

			return;
		}

		g_counter_CONFIG_HOUR_CONFIG_DATE++;

		reset_UART_buffer_values(UART);

		set_UART_flag_false(UART);
	}

}


void read_hr(UART_Type *UART)
{
	display_read_hr(UART);

    set_UART_flag_false(UART);
}

void read_date(UART_Type *UART)
{
	RTC_read_device();

	display_date(UART);

	set_UART_flag_false(UART);
}

void config_hr(UART_Type *UART)
{
	display_config_hr(UART);

	set_UART_flag_false(UART);
}

void config_date(UART_Type *UART)
{
	display_config_date(UART);

	set_UART_flag_false(UART);
}

void read_temp(UART_Type *UART)
{
	if(read_sensor_temp() == kStatus_Success )
	{
		display_read_temp(UART);
	}
	else
	{
		I2C_device_fail_communication(UART);
	}

	set_UART_flag_false(UART);
}

void read_hum(UART_Type *UART)
{
	if(read_sensor_hum() == kStatus_Success )
	{
		display_read_hum(UART);
	}
	else
	{
		I2C_device_fail_communication(UART);
	}

	set_UART_flag_false(UART);
}

void activate_log(UART_Type *UART)
{
	display_active_log(UART);
}

void read_log(UART_Type *UART)
{
	display_read_log(UART);
}

void read_log_from_memory1(UART_Type *UART)
{

	uint8_t max_capture_number = get_max_counter_capture(UART);
	uint8_t mem_adress_access;
	uint8_t i = 0;
	uint8_t j = 0;

	if(UART == UART0)
	{
		mem_adress_access = mem_adress_access_initial_value;
	}
	if(UART == UART4)
	{
		mem_adress_access = mem_adress_access_initial_value2;
	}

	clear_screen(UART);

	reading_log_repositioning(UART);

	for(i = 0; i < max_capture_number; i++)
	{
		read_mem(mem_adress_access, g_capture_data_length, UART);

		mem_adress_access = mem_adress_access + g_capture_data_length;

		display_read_log_from_memory(UART);

		reading_log_repositioning(UART);

		for(j = 0; j < i+1; j++)
		{
			move_down_cursor(UART);
		}
	}
}

uint8_t Get_Menu_input(UART_Type *UART)
{
	uint8_t input = ZERO;

	if(ESC == get_data(PLACE0, UART) || ESC == get_data(PLACE1, UART))
	{
		input = Esc;
		reset_UART_buffer_values(UART);
		return input;
	}

	if(BOTON_1 == get_data(PLACE0, UART) && ENTER == get_data(PLACE1, UART))
	{
		input = Read_HR;
		reset_UART_buffer_values(UART);
		return input;
	}

	if(BOTON_2 == get_data(PLACE0, UART) && ENTER == get_data(PLACE1, UART))
	{
		input = Config_HR;
		reset_UART_buffer_values(UART);
		return input;
	}

	if(BOTON_3 == get_data(PLACE0, UART) && ENTER == get_data(PLACE1, UART))
	{
		input = Read_date;
		reset_UART_buffer_values(UART);
		return input;
	}

	if(BOTON_4 == get_data(PLACE0, UART) && ENTER == get_data(PLACE1, UART))
	{
		input = Congif_date;
		reset_UART_buffer_values(UART);
		return input;
	}

	if(BOTON_5 == get_data(PLACE0, UART) && ENTER == get_data(PLACE1, UART))
	{
		input = Read_temperature;
		reset_UART_buffer_values(UART);
		return input;
	}

	if(BOTON_6 == get_data(PLACE0, UART) && ENTER == get_data(PLACE1, UART))
	{
		input = Read_Humidity;
		reset_UART_buffer_values(UART);
		return input;
	}

	if(BOTON_7 == get_data(PLACE0, UART) && ENTER == get_data(PLACE1, UART))
	{
		input = Activate_Log;
		reset_UART_buffer_values(UART);
		reset_UART_buffer_values_log(UART);
		return input;
	}

	if(BOTON_8 == get_data(PLACE0, UART) && ENTER == get_data(PLACE1, UART))
	{
		input = Read_Log;
		reset_UART_buffer_values(UART);
		reset_UART_buffer_values_log(UART);
		return input;
	}

	return AnythingElse;
}

uint8_t Get_Log_input(UART_Type *UART)
{
	uint8_t input = ZERO;

	if(Yes == get_data_log(PLACE0, UART) && ENTER == get_data_log(PLACE1, UART))
	{
		input = YES;
		reset_UART_buffer_values_log(UART);
		return input;
	}
	if(No == get_data_log(PLACE0, UART) && ENTER == get_data_log(PLACE1, UART))
	{
		input = NO;
		reset_UART_buffer_values_log(UART);
		return input;
	}
	return AnythingElse;
}
