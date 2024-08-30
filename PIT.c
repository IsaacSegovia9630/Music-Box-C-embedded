/*
 * PIT.c
 *
 *  Created on: 12 sep 2022
 *      Author: Tahirí Ramos
 	\brief
		This is the source file for the PIT configurations.
		It contains some functions to configure and start the PIT0.

	\authors: Hegui Tahirí Ramos Ugalde
	          Isaac Segovia Hernández
	\date	12/09/2022
	\to do
	    Configure all the PIT's remaining.
 */

#include "PIT.h"

/*Callbacks for each channel*/
static void (*PIT_CH0_callback)(void) = 0;
static void (*PIT_CH1_callback)(void) = 0;
static void (*PIT_CH2_callback)(void) = 0;
static void (*PIT_CH3_callback)(void) = 0;

void pit_ch0_init(){

	uint32_t clock;
	clock = CLOCK_GetFreq(kCLOCK_BusClk);
	//This Function makes that systems levels clocks are in allowed range
	CLOCK_SetSimSafeDivs();

	//Structure of initialize PIT
	pit_config_t pitConfig;

	//Initialize the configuration structure of the PIT
	PIT_GetDefaultConfig(&pitConfig);

	//Initialize PIT module. In this case it initialize the Module Control Register
	PIT_Init(PIT_BASE_ADDRESS,&pitConfig);

	//Set timer Period for channel 0 (LDVAL0 register)
	PIT_SetTimerPeriod(PIT, PIT_CH_0, USEC_TO_COUNT(WAIT_TIME, clock));

	//Enable timer interrupts for channel (TCTRL0 register)
	PIT_EnableInterrupts(PIT_BASE_ADDRESS, PIT_CH_0, kPIT_TimerInterruptEnable);

	//Enable at the NVIC
	EnableIRQ(PIT0_IRQ_ID);
}

void pit_init(UART_Type *UART, uint8_t time)
{

	float32_t capture_time = time * 10500000;

	if(UART == UART0)
	{
		//This Function makes that systems levels clocks are in allowed range
		CLOCK_SetSimSafeDivs();

		//Structure of initialize PIT
		pit_config_t pitConfig;

		//Initialize the configuration structure of the PIT
		PIT_GetDefaultConfig(&pitConfig);

		//Initialize PIT module. In this case it initialize the Module Control Register
		PIT_Init(PIT_BASE_ADDRESS,&pitConfig);

		//Set timer Period for channel 0 (LDVAL0 register)
		PIT_SetTimerPeriod(PIT, PIT_CH_1, capture_time);

		//Enable timer interrupts for channel (TCTRL0 register)
		PIT_EnableInterrupts(PIT_BASE_ADDRESS, PIT_CH_1, kPIT_TimerInterruptEnable);

		//Enable at the NVIC
		EnableIRQ(PIT1_IRQ_ID);

		PIT_StartTimer(PIT_BASE_ADDRESS, PIT_CH_1);
	}

	if(UART == UART4)
	{
		//This Function makes that systems levels clocks are in allowed range
		CLOCK_SetSimSafeDivs();

		//Structure of initialize PIT
		pit_config_t pitConfig;

		//Initialize the configuration structure of the PIT
		PIT_GetDefaultConfig(&pitConfig);

		//Initialize PIT module. In this case it initialize the Module Control Register
		PIT_Init(PIT_BASE_ADDRESS,&pitConfig);

		//Set timer Period for channel 0 (LDVAL0 register)
		PIT_SetTimerPeriod(PIT, PIT_CH_2, capture_time);

		//Enable timer interrupts for channel (TCTRL0 register)
		PIT_EnableInterrupts(PIT_BASE_ADDRESS, PIT_CH_2, kPIT_TimerInterruptEnable);

		//Enable at the NVIC
		EnableIRQ(PIT2_IRQ_ID);

		PIT_StartTimer(PIT_BASE_ADDRESS, PIT_CH_2);
	}

}

void pit_ch1_stop(){

	    //Stop the timer
	    PIT_StopTimer(PIT_BASE_ADDRESS, PIT_CH_1);

		//Disable at the NVIC
	    DisableIRQ(PIT1_IRQ_ID);

		//Disable timer interrupts for channel
	    PIT_DisableInterrupts(PIT_BASE_ADDRESS, PIT_CH_1, kPIT_TimerInterruptEnable);

}

void pit_ch2_stop(){

	    //Stop the timer
	    PIT_StopTimer(PIT_BASE_ADDRESS, PIT_CH_2);

		//Disable at the NVIC
	    DisableIRQ(PIT2_IRQ_ID);

		//Disable timer interrupts for channel
	    PIT_DisableInterrupts(PIT_BASE_ADDRESS, PIT_CH_2, kPIT_TimerInterruptEnable);

}

void PIT_callback_init(PIT_timer_t pit, void(*handler)(void))
{
	switch (pit)
	{
	case PIT0:
		PIT_CH0_callback = handler;
		break;
	case PIT1:
		PIT_CH1_callback = handler;
		break;
	case PIT2:
		PIT_CH2_callback = handler;
		break;
	case PIT3:
		PIT_CH3_callback = handler;
		break;
	default:
		break;
	}
}/*PIT_callback_init */

void PIT0_IRQHandler(void)
{

    if(PIT_CH0_callback)
	{
		PIT_CH0_callback();
	}

    /* Clear interrupt flag.*/
    PIT_ClearStatusFlags(PIT_BASE_ADDRESS, PIT_CH_0, kPIT_TimerFlag);
    __DSB();
}

void PIT1_IRQHandler(void)
{

    if(PIT_CH1_callback)
	{
		PIT_CH1_callback();
	}
    /* Clear interrupt flag.*/
    PIT_ClearStatusFlags(PIT_BASE_ADDRESS, PIT_CH_1, kPIT_TimerFlag);
    __DSB();
}

void PIT2_IRQHandler(void)
{

    if(PIT_CH2_callback)
	{
		PIT_CH2_callback();
	}
    /* Clear interrupt flag.*/
    PIT_ClearStatusFlags(PIT_BASE_ADDRESS, PIT_CH_2, kPIT_TimerFlag);
    __DSB();
}
