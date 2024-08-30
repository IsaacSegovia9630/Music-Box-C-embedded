/*
 * 	Created by Tahirí Ramos and Isaac Segovia
 * 	12/10/2022
 *
 * 	Practice Definition:
 *
 *  Consiste en la implementación de panel electrónico de mensajes.
 *
 */

#include "UART.h"
#include "MENU.h"
#include "GPIO.h"
#include "NVIC.h"
#include "I2C.h"
#include "bits.h"
#include "RTC.h"
#include "SENSOR_TEMP_HUM.h"
#include "PIT.h"

int main(void)
{
	//Initialize GPIO basics
	gpio_init();

	//Initialize UART0 and UART4
	init_UART();

	//Initialize Pins that controls I2C device
	I2C_init();

	//Initialize the firts write and reading on the RTC device
	RTC_init();

	GPIO_callback_init(GPIO_D, RTC_read_device);

	PIT_callback_init(PIT1, data_capture1);

	PIT_callback_init(PIT2, data_capture2);

	NVIC_set_basepri_threshold(PRIORITY_11);

	reset_terminal();

	change_display_attributes_terminal();

	display_menu(UART0);

	display_menu(UART4);

	pit_ch0_init();

	sensor_restart();

	sensor_init();

	sensor_read_comands();

    while(TRUE)
    {
        if(get_UART0_flag())
        {
            select_menu(UART0,UART_0);
        }

        if(get_UART4_flag())
        {
            select_menu(UART4,UART_4);
        }
    }
    return 0 ;
}
