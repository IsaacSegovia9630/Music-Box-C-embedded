/*
 * PIT.h
 *
 *  Created on: 12 sep 2022
 *      Author: tahir
 */

#ifndef EQUIPO_6_T5_PIT_H_
#define EQUIPO_6_T5_PIT_H_

#include "fsl_pit.h"
#include "fsl_common_arm.h"
#include "MK64F12.h"
#include "stdint.h"

#define PIT0_IRQ_ID  		PIT0_IRQn
#define PIT1_IRQ_ID  		PIT1_IRQn
#define PIT2_IRQ_ID  		PIT2_IRQn
#define CYCLES_FOR_2SECONDS 21000000
#define PIT_BASE_ADDRESS 	PIT
#define PIT_CH_0 			kPIT_Chnl_0
#define PIT_CH_1 			kPIT_Chnl_1
#define PIT_CH_2 			kPIT_Chnl_2
#define WAIT_TIME 			10000u

typedef enum
{
	PIT0,
	PIT1,
	PIT2,
	PIT3
} PIT_timer_t;

typedef float float32_t;

void pit_ch0_init();

void pit_init(UART_Type *UART, uint8_t time);

void pit_ch1_stop();

void pit_ch2_stop();

void data_capture1(void);

void data_capture2(void);

void set_max_data_capture(UART_Type *UART, uint8_t capture);

void PIT_callback_init(PIT_timer_t pit, void(*handler)(void));

void frequency_state();



#endif /* EQUIPO_6_T5_PIT_H_ */
