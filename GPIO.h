/*
 * GPIO.h
 *
 *  Created on: 12 sep 2022
 *      Author: Tahirí Ramos
 */

#ifndef EQUIPO_6_T5_GPIO_H_
#define EQUIPO_6_T5_GPIO_H_

#include "fsl_gpio.h"
#include "fsl_port.h"

void gpio_init(void);

/*! These constants are used to select an specific port in the different API functions*/
typedef enum{GPIO_A, /*!< Definition to select GPIO A */
			 GPIO_B, /*!< Definition to select GPIO B */
			 GPIO_C, /*!< Definition to select GPIO C */
			 GPIO_D, /*!< Definition to select GPIO D */
			 GPIO_E, /*!< Definition to select GPIO E */
			} gpio_name_t;

void GPIO_callback_init(gpio_name_t gpio, void (*handler)(void));

typedef struct
{
   uint8_t flag_port_a : 1;
   uint8_t flag_port_b : 1;
   uint8_t flag_port_c : 1;
   uint8_t flag_port_d : 1;
   uint8_t flag_port_e : 1;
} gpio_interrupt_flags_t;

void I2C_pins_init(void);

#endif /* EQUIPO_6_T5_GPIO_H_ */
