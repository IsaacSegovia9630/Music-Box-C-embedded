/*
 * GPIO.c
 *
 *  Created on: 12 sep 2022
 *      Author: Tahirí Ramos
 */

#include "GPIO.h"
#include "bits.h"
#include "RTC.h"
#include "pin_mux.h"
#include "NVIC.h"
/*
 * 	Initialize GPIO for the peripherals used in this project
 */

#define Display_Teen_Hours (1 << bit_0)
#define Display_Hours (1 << bit_1)
#define Display_Teen_Minutes (1 << bit_2)
#define Display_Minutes (1 << bit_3)
#define Display_Teen_Seconds (1 << bit_4)
#define Display_Seconds (1 << bit_5)


const gpio_pin_config_t gpio_input_config = {
			kGPIO_DigitalInput,
			0,
			    };

const gpio_pin_config_t gpio_output_config = {
			kGPIO_DigitalOutput,
			1,
			};

const port_pin_config_t button_config = {
			kPORT_PullUp,                                            /* Internal pull-up resistor is enabled */
			kPORT_FastSlewRate,                                      /* Fast slew rate is configured */
			kPORT_PassiveFilterEnable,                              /* Passive filter is disabled */
			kPORT_OpenDrainDisable,                                  /* Open drain is disabled */
			kPORT_HighDriveStrength,                                 /* High drive strength is configured */
			kPORT_MuxAsGpio,                                         /* Pin is configured as PTA4 */
			kPORT_UnlockRegister                                     /* Pin Control Register fields [15:0] are not locked */
		  };

const port_pin_config_t portb_pin2_config =
{/* Internal pull-up resistor is enabled */
			kPORT_PullUp,
			/* Fast slew rate is configured */
			kPORT_FastSlewRate,
			/* Passive filter is disabled */
			kPORT_PassiveFilterDisable,
			/* Open drain is enabled */
			kPORT_OpenDrainEnable,
			/* Low drive strength is configured */
			kPORT_LowDriveStrength,
			/* Pin is configured as I2C0_SCL */
			kPORT_MuxAlt2,
			/* Pin Control Register fields [15:0] are not locked */
			kPORT_UnlockRegister
};

const port_pin_config_t portb_pin3_config =
{/* Internal pull-up resistor is enabled */
			kPORT_PullUp,
			/* Fast slew rate is configured */
			kPORT_FastSlewRate,
			/* Passive filter is disabled */
			kPORT_PassiveFilterDisable,
			/* Open drain is enabled */
			kPORT_OpenDrainEnable,
			/* Low drive strength is configured */
			kPORT_LowDriveStrength,
			/* Pin is configured as I2C0_SDA */
			kPORT_MuxAlt2,
			/* Pin Control Register fields [15:0] are not locked */
			kPORT_UnlockRegister
};

static void (*gpio_A_callback)(void) = 0;
static void (*gpio_B_callback)(void) = 0;
static void (*gpio_C_callback)(void) = 0;
static void (*gpio_D_callback)(void) = 0;
static void (*gpio_E_callback)(void) = 0;

//Callback's initialization for GPIO's
void GPIO_callback_init(gpio_name_t gpio, void (*handler)(void))
{
	switch (gpio){
	    case GPIO_A:
	    	gpio_A_callback = handler;
	    	break;

	    case GPIO_B:
	    	gpio_B_callback = handler;
	    	break;

	    case GPIO_C:
	    	gpio_C_callback = handler;
	    	break;

	    case GPIO_D:
	    	gpio_D_callback = handler;
	    	break;

	    case GPIO_E:
	    	gpio_E_callback = handler;
	    	break;
	}
}

//SW3 Handler
void PORTA_IRQHandler(void)
{
	uint32_t irq_status = 0;

	irq_status = GPIO_PortGetInterruptFlags(GPIOA);

	//Code to execute

	GPIO_PortClearInterruptFlags(GPIOA, irq_status);
}

//Config.Time, Hour/Start, Min/Stop, and Clear Handler
void PORTB_IRQHandler(void)
{

	uint32_t irq_status = 0;

	irq_status = GPIO_PortGetInterruptFlags(GPIOB);


	//Code to execute

	GPIO_PortClearInterruptFlags(GPIOB, irq_status);
}


//SW2, Time/Stopwatch, Time/Alarm and Stop Handler
void PORTC_IRQHandler(void)
{
	uint32_t irq_status = 0;

	irq_status = GPIO_PortGetInterruptFlags(GPIOC);

	//Code to execute

	GPIO_PortClearInterruptFlags(GPIOC, irq_status);
}

void PORTD_IRQHandler(void)
{
	uint32_t irq_status = 0;

	irq_status = GPIO_PortGetInterruptFlags(GPIOD);

	/*if(gpio_D_callback)
	{
		gpio_D_callback();
	}*/
	RTC_read_device();

	GPIO_PortClearInterruptFlags(GPIOD, irq_status);
}

// GPIO initialization
void gpio_init(void)
{
		CLOCK_EnableClock(kCLOCK_PortA);
		CLOCK_EnableClock(kCLOCK_PortB);
		CLOCK_EnableClock(kCLOCK_PortC);
		CLOCK_EnableClock(kCLOCK_PortE);
		CLOCK_EnableClock(kCLOCK_PortD);

		//Initialize SW2
		GPIO_PinInit(GPIOC, bit_6, &gpio_input_config);
		PORT_SetPinConfig(PORTC, bit_6, &button_config);
		PORT_SetPinInterruptConfig(PORTC, bit_6, kPORT_InterruptFallingEdge);

		//Initialize SW3
		GPIO_PinInit(GPIOA, bit_4, &gpio_input_config);
		PORT_SetPinConfig(PORTA, bit_4, &button_config);
		PORT_SetPinInterruptConfig(PORTA, bit_4, kPORT_InterruptFallingEdge);

		//Initialize red LED
		PORT_SetPinMux(PORTB, bit_22, kPORT_MuxAsGpio);
		GPIO_PinInit(GPIOB, bit_22, &gpio_output_config);

		//Initialize blue LED
		PORT_SetPinMux(PORTB, bit_21, kPORT_MuxAsGpio);
		GPIO_PinInit(GPIOB, bit_21, &gpio_output_config);

		//Initialize green LED
		PORT_SetPinMux(PORTE, bit_26, kPORT_MuxAsGpio);
		GPIO_PinInit(GPIOE, bit_26, &gpio_output_config);
}

void I2C_pins_init(void)
{
	/* PORTB (pin 2) is configured as I2C0_SCL */
	PORT_SetPinConfig(PORTB, bit_2, &portb_pin2_config);

	/* PORTB (pin 3) is configured as I2C0_SDA */
	PORT_SetPinConfig(PORTB, bit_3, &portb_pin3_config);

	//Connection between SQW and PTD0
	GPIO_PinInit(GPIOD, bit_0, &gpio_input_config);
	PORT_SetPinConfig(PORTD, bit_0, &button_config);
	PORT_SetPinInterruptConfig(PORTD, bit_0, kPORT_InterruptFallingEdge);

	/**Enables and sets a particular interrupt and its priority*/
	NVIC_enable_interrupt_and_priority(PORTD_IRQ,PRIORITY_2);

    SIM->SOPT5 = ((SIM->SOPT5 &
                   /* Mask bits to zero which are setting */
                   (~(SIM_SOPT5_UART0TXSRC_MASK)))

                  /* UART 0 transmit data source select: UART0_TX pin. */
                  | SIM_SOPT5_UART0TXSRC(SOPT5_UART0TXSRC_UART_TX));
}

