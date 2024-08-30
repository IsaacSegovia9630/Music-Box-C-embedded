/*
 * UART.c
 *
 *  Created on: 11 oct 2022
 *      Author: Tahirí Ramos
 */
#include "fsl_port.h"
#include "fsl_uart.h"
#include "UART.h"
#include "MENU.h"
#include "bits.h"
#include "NVIC.h"
#include "RTC.h"

typedef struct
{
	uint8_t	flag;	    //indica que hay datos nuevos
	uint8_t mail_box;	//contiene los datos recibidos

}UART_mail_box_t;

static uint8_t time_FINAL[I2C_DATA_LENGTH + NULL_CHARACTER] = {0,0,':',0,0,':',0,0};
static uint8_t date_FINAL[DATE_LENGTH + NULL_CHARACTER]     = {0,0,'/',0,0,'/'};
static uint8_t date_FINAL2[DATE_LENGTH2 + NULL_CHARACTER]   = {'2','0',0,0};
static uint8_t temp_FINAL[DATE_LENGTH3 + NULL_CHARACTER]    = {0,0,'.',0,0,'C'};
static uint8_t hum_FINAL[DATE_LENGTH4 + NULL_CHARACTER]     = {0,'%'};

static uint8_t DATA_FINAL_TEMP[DATA_FINAL_TEMP_length + NULL_CHARACTER]   = {'T','e','m','p','e','r','a','t','u','r','a',':',0,0,'.',0,0,'C',','};
static uint8_t DATA_FINAL_HUM[DATA_FINAL_HUM_length + NULL_CHARACTER]     = {'H','u','m','e','d','a','d',':',0,'%',','};
static uint8_t DATA_FINAL_HRS[DATA_FINAL_HRS_length + NULL_CHARACTER]     = {0,0,':',0,0,':',0,0,' ','h','r','s',' '};
static uint8_t DATA_FINAL_DATE[DATA_FINAL_DATE_length + NULL_CHARACTER]   = {0,0,'/',0,0,'/','2','0',0,0};

static uint8_t DATA_FINAL_TEMP2[DATA_FINAL_TEMP_length + NULL_CHARACTER]   = {'T','e','m','p','e','r','a','t','u','r','a',':',0,0,'.',0,0,'C',','};
static uint8_t DATA_FINAL_HUM2[DATA_FINAL_HUM_length + NULL_CHARACTER]     = {'H','u','m','e','d','a','d',':',0,'%',','};
static uint8_t DATA_FINAL_HRS2[DATA_FINAL_HRS_length + NULL_CHARACTER]     = {0,0,':',0,0,':',0,0,' ','h','r','s',' '};
static uint8_t DATA_FINAL_DATE2[DATA_FINAL_DATE_length + NULL_CHARACTER]   = {0,0,'/',0,0,'/','2','0',0,0};


UART_mail_box_t g_mail_box_uart_0 ={0, 0};
UART_mail_box_t g_mail_box_uart_4 ={0, 0};

uint8_t g_received_data_UART0[RECEIVED_DATA_BUFFER_SIZE];
uint8_t g_data_place_UART0 = 0;

uint8_t g_received_data_UART4[RECEIVED_DATA_BUFFER_SIZE];
uint8_t g_data_place_UART4 = 0;

uint8_t g_received_data_UART0_log[RECEIVED_DATA_BUFFER_SIZE];
uint8_t g_data_place_UART0_log = 0;
uint8_t g_received_data_UART4_log[RECEIVED_DATA_BUFFER_SIZE];
uint8_t g_data_place_UART4_log = 0;

//The next command resets the terminal to the inital state (black screen and cursor at the top left)
uint8_t g_vt100_reset[]       = "\033c";
//The next command change the font
uint8_t g_vt100_change_font[] = "\033[3;35;40m";
//The next command clear all characters from the screen
uint8_t g_vt100_clear_screen[] = "\033[2J";
//The next command clear the line
uint8_t g_vt100_clear_line[] = "\033[K";
//The next command force the cursor to be in a certain place
uint8_t g_vt100_force_cursor[] = "\033[{ROW};{COLUMN}f"; //Replace Row and Column
//The next command move the cursor up "COUNT" times
uint8_t g_vt100_up_cursor[] = "\033[{COUNT}A"; //Replace COUNT
//The next command move the cursor down "COUNT" times
uint8_t g_vt100_down_cursor[] = "\033[B";//Replace COUNT
//The next command move the cursor forward "COUNT" times
uint8_t g_vt100_forward_cursor[] = "\033[{COUNT}C";//Replace COUNT
//The next command move the cursor backwards "COUNT" times
uint8_t g_vt100_backward_cursor[] = "\033[{COUNT}D";//Replace COUNT

/* -------------------------------------Menu Princiapl-------------------------------------*/
/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_MP_POS1[]  = "\033[5;10H";
uint8_t g_vt100_MP_TEXT1[] = "1) Leer Hora\r";

/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_MP_POS2[]  = "\033[6;10H";
uint8_t g_vt100_MP_TEXT2[] = "2) Configurar Hora\r";

/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_MP_POS3[]  = "\033[7;10H";
uint8_t g_vt100_MP_TEXT3[] = "3) Leer Fecha\r";

/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_MP_POS4[]  = "\033[8;10H";
uint8_t g_vt100_MP_TEXT4[] = "4) Configurar Fecha\r";

/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_MP_POS5[]  = "\033[9;10H";
uint8_t g_vt100_MP_TEXT5[] = "5) Leer Temperatura\r";

/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_MP_POS6[]  = "\033[10;10H";
uint8_t g_vt100_MP_TEXT6[] = "6) Leer Humedad\r";

/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_MP_POS7[]  = "\033[11;10H";
uint8_t g_vt100_MP_TEXT7[] = "7) Activar Log\r";

/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_MP_POS8[]  = "\033[12;10H";
uint8_t g_vt100_MP_TEXT8[] = "8) Leer Log\r";

uint8_t g_vt100_MP_POS9[]  = "\033[13;10H";



/* --------------------------------------USO DE RECURSO-------------------------------------*/

uint8_t g_vt100_Resource_POS1[]  = "\033[15;10H";
uint8_t g_vt100_Resource_TEXT1[] = "Este recurso esta siendo usado por la otra terminal!\r";
uint8_t g_vt100_Resource_POS2[]  = "\033[16;10H";

/* --------------------------------------Fallo de comunicación con I2C------------------------*/

uint8_t g_vt100_I2C_Fail_POS1[]   = "\033[10;10H";
uint8_t g_vt100_I2C_Fail_TEXT1[]  = "ERROR: Sin comunicacion con dispositivo I2C\r";
uint8_t g_vt100_I2C_Fail_POS2[]   = "\033[11;10H";

/* --------------------------------------Actualizar Hora-------------------------------------*/

uint8_t g_vt100_UpdateHour_POS1[]   = "\033[10;10H";
uint8_t g_vt100_UpdateHour_TEXT1[]  = "La hora actual es:\r";
uint8_t g_vt100_UpdateHour_POS2[]   = "\033[11;10H";

/* --------------------------------------Confirmación de Actualizar Hora-------------------------------------*/

uint8_t g_vt100_Confirmation_HR_POS1[]   = "\033[11;10H";
uint8_t g_vt100_Confirmation_HR_TEXT1[]  = "La hora ha sido cambiada...\r";
uint8_t g_vt100_Confirmation_HR_POS2[]   = "\033[12;10H";

/* --------------------------------------Confirmación de Actualizar Fecha-------------------------------------*/

uint8_t g_vt100_Confirmation_DATE_POS1[]   = "\033[12;10H";
uint8_t g_vt100_Confirmation_DATE_TEXT1[]  = "La fecha ha sido cambiada...\r";
uint8_t g_vt100_Confirmation_DATE_POS2[]   = "\033[13;10H";

/* --------------------------------------Leer Fecha-------------------------------------*/

uint8_t g_vt100_ReadDate_POS1[]   = "\033[10;10H";
uint8_t g_vt100_ReadDate_TEXT1[]  = "La fecha actual es:\r";
uint8_t g_vt100_ReadDate_POS2[]   = "\033[11;10H";
/* --------------------------------------Configurar Fecha-------------------------------------*/

uint8_t g_vt100_ConfigDate_POS1[]   = "\033[10;10H";
uint8_t g_vt100_ConfigDate_TEXT1[]  = "Escribir fecha en dd/mm/aa:\r";
uint8_t g_vt100_ConfigDate_POS2[]   = "\033[11;10H";

/* --------------------------------------Leer Temperatura-------------------------------------*/

uint8_t g_vt100_ReadTemp_POS1[]   = "\033[10;10H";
uint8_t g_vt100_ReadTemp_TEXT1[]  = "La temperatura es:\r";
uint8_t g_vt100_ReadTemp_POS2[]   = "\033[11;10H";

/* --------------------------------------Leer Humedad-------------------------------------*/

uint8_t g_vt100_ReadHum_POS1[]   = "\033[10;10H";
uint8_t g_vt100_ReadHum_TEXT1[]  = "La humedad es:\r";
uint8_t g_vt100_ReadHum_POS2[]   = "\033[11;10H";

/* --------------------------------------Confirmación de Activar log-------------------------------------*/

uint8_t g_vt100_Confirmation_LOG_POS1[]   = "\033[12;10H";
uint8_t g_vt100_Confirmation_LOG_TEXT1[]  = "Deseas activar la captura de datos?: Si-->y No-->n \r";
uint8_t g_vt100_Confirmation_LOG_POS2[]   = "\033[13;10H";

/* --------------------------------------Tiempo de captura log-------------------------------------*/

uint8_t g_vt100_TimeCapture_LOG_POS1[]   = "\033[12;10H";
uint8_t g_vt100_TimeCapture_LOG_TEXT1[]  = "Introducir tiempo de captura en segundos en formato 00\r";
uint8_t g_vt100_TimeCapture_LOG_POS2[]   = "\033[13;10H";

/* --------------------------------------Cantidad de captura log-------------------------------------*/

uint8_t g_vt100_quantityCapture_LOG_POS1[]   = "\033[12;10H";
uint8_t g_vt100_quantityCapture_LOG_TEXT1[]  = "Introducir numero de capturas en fotmato 00: \r";
uint8_t g_vt100_quantityCapture_LOG_POS2[]   = "\033[13;10H";

/* --------------------------------------Cantidad de captura log-------------------------------------*/

uint8_t g_vt100_StartingCapture_LOG_POS1[]   = "\033[12;10H";
uint8_t g_vt100_StartingCapture_LOG_TEXT1[]  = "Se estan empezando las capturas...\r";
uint8_t g_vt100_StartingCapture_LOG_POS2[]   = "\033[13;10H";

/* --------------------------------------Lectura de log-------------------------------------*/

uint8_t g_vt100_Read_LOG_POS1[]   = "\033[12;10H";
uint8_t g_vt100_Read_LOG_TEXT1[]  = "Deseas leer la captura de datos?: Si-->Y No-->N\r";
uint8_t g_vt100_Read_LOG_POS2[]   = "\033[13;10H";

/* --------------------------------------Lectura de log de la meoria-------------------------------------*/

uint8_t g_vt100_Read_MEMORY_LOG_POS1[]   = "\033[12;10H";

/* --------------------------------------1) LEER HORA-------------------------------------*/

uint8_t g_vt100_8[] = "\033[0;34;43m";
/*VT100 command for clearing the screen*/
uint8_t g_vt100_9[] = "\033[2J";
/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_10[] = "\033[10;10H";
uint8_t g_vt100_11[] = "1) La hora actual es:\r";
/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_12[] = "\033[11;10H";
uint8_t g_vt100_13[] =  "   \r";

/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_15[] = "\033[11;13H";
/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_15_1[] = "\033[11;15H";

/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_16[] = "\033[11;15H";
/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_17[] = ":\r";
/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_19[] = "\033[11;16H";
/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_19_1[] = "\033[11;17H";

/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_20[] = "\033[11;18H";
/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_21[] = ":\r";
/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_23[] = "\033[11;19H";
/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_23_1[] = "\033[11;20H";

/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_24[] = "\033[12;10H";

/* ----------------------------------2) CONFIGURAR HORA-------------------------------------*/

/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_ConfigHr_POS1[] = "\033[9;10H";
uint8_t g_vt100_ConfigHr_TEXT1[] = "Escribir hora en hh:mm:ss\r";
/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_ConfigHr_POS2[] = "\033[10;10H";

void UART0_RX_TX_IRQHandler(void)
{
    /* If new data arrived. */
    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(UART0))
    {
    	g_received_data_UART0[g_data_place_UART0] = UART_ReadByte(UART0);

    	g_received_data_UART0_log[g_data_place_UART0_log] = g_received_data_UART0[g_data_place_UART0];

    	UART_WriteBlocking(UART0, &g_received_data_UART0[g_data_place_UART0] , UART_DATA_LENGTH);

    	g_data_place_UART0++;
    	g_data_place_UART0_log++;

    	if(g_data_place_UART0 >= 2)
    	{
    		g_data_place_UART0 = 0;
    	}
    	if(g_data_place_UART0_log >= 2)
    	{
    		g_data_place_UART0_log = 0;
    	}


    	g_mail_box_uart_0.flag = true;

    }
    SDK_ISR_EXIT_BARRIER;
}

void UART4_RX_TX_IRQHandler(void)
{
    /* If new data arrived. */
    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(UART4))
    {
    	g_received_data_UART4[g_data_place_UART4] = UART_ReadByte(UART4);

    	g_received_data_UART4_log[g_data_place_UART4_log] = g_received_data_UART4[g_data_place_UART4];

    	UART_WriteBlocking(UART4, &g_received_data_UART4[g_data_place_UART4] , UART_DATA_LENGTH);

    	g_data_place_UART4++;
    	g_data_place_UART4_log++;

    	if(g_data_place_UART4 >= 2)
    	{
    		g_data_place_UART4 = 0;
    	}
    	if(g_data_place_UART4_log >= 2)
    	{
    		g_data_place_UART4_log = 0;
    	}

    	g_mail_box_uart_4.flag = true;

    }
    SDK_ISR_EXIT_BARRIER;
}

void init_UART(void)
{

    uart_config_t config;
    uint32_t uart_clock;

    //Initialization for UART 0 BEGINS here:
    CLOCK_EnableClock(kCLOCK_PortB);

    //UART0_RX
    PORT_SetPinMux(PORTB, bit_16, kPORT_MuxAlt3);
    //UART0_TX
    PORT_SetPinMux(PORTB, bit_17, kPORT_MuxAlt3);


    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = UART_BAUDRATE;
    config.enableTx     = TRUE;
    config.enableRx     = TRUE;

    uart_clock = CLOCK_GetFreq(UART0_CLK_SRC);
    UART_Init(UART0, &config, uart_clock);

    UART_EnableInterrupts(UART0, kUART_RxDataRegFullInterruptEnable |
    kUART_RxOverrunInterruptEnable);
	NVIC_enable_interrupt_and_priority(UART0_IRQ, PRIORITY_2);
	//Initialization for UART 0 ENDS here.


    //Initialization for UART 4 BEGINS here:
    CLOCK_EnableClock(kCLOCK_PortC);

    //UART4_RX
    PORT_SetPinMux(PORTC, bit_14, kPORT_MuxAlt3);
    //UART4_TX
    PORT_SetPinMux(PORTC, bit_15, kPORT_MuxAlt3);

    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = UART_BAUDRATE;
    config.enableTx     = TRUE;
    config.enableRx     = TRUE;

    uart_clock = CLOCK_GetFreq(UART4_CLK_SRC);
    UART_Init(UART4, &config, uart_clock);

    UART_EnableInterrupts(UART4, kUART_RxDataRegFullInterruptEnable |
    kUART_RxOverrunInterruptEnable);
	NVIC_enable_interrupt_and_priority(UART4_IRQ, PRIORITY_1);
	//Initialization for UART 4 ENDS here.

	NVIC_global_enable_interrupts;
}

void display_menu(UART_Type *UART)
{
	UART_WriteBlocking(UART, g_vt100_clear_screen, get_data_length(g_vt100_clear_screen));

	UART_WriteBlocking(UART,g_vt100_change_font , get_data_length(g_vt100_change_font));

	UART_WriteBlocking(UART, g_vt100_MP_POS1, get_data_length(g_vt100_MP_POS1));
	UART_WriteBlocking(UART, g_vt100_MP_TEXT1, get_data_length(g_vt100_MP_TEXT1));

	UART_WriteBlocking(UART, g_vt100_MP_POS2, get_data_length(g_vt100_MP_POS2));
	UART_WriteBlocking(UART, g_vt100_MP_TEXT2, get_data_length(g_vt100_MP_TEXT2));

	UART_WriteBlocking(UART, g_vt100_MP_POS3, get_data_length(g_vt100_MP_POS3));
	UART_WriteBlocking(UART, g_vt100_MP_TEXT3, get_data_length(g_vt100_MP_TEXT3));

	UART_WriteBlocking(UART, g_vt100_MP_POS4, get_data_length(g_vt100_MP_POS4));
	UART_WriteBlocking(UART, g_vt100_MP_TEXT4, get_data_length(g_vt100_MP_TEXT4));

	UART_WriteBlocking(UART, g_vt100_MP_POS5, get_data_length(g_vt100_MP_POS5));
	UART_WriteBlocking(UART, g_vt100_MP_TEXT5, get_data_length(g_vt100_MP_TEXT5));

	UART_WriteBlocking(UART, g_vt100_MP_POS6, get_data_length(g_vt100_MP_POS6));
	UART_WriteBlocking(UART, g_vt100_MP_TEXT6, get_data_length(g_vt100_MP_TEXT6));

	UART_WriteBlocking(UART, g_vt100_MP_POS7, get_data_length(g_vt100_MP_POS7));
	UART_WriteBlocking(UART, g_vt100_MP_TEXT7, get_data_length(g_vt100_MP_TEXT7));

	UART_WriteBlocking(UART, g_vt100_MP_POS8, get_data_length(g_vt100_MP_POS8));
	UART_WriteBlocking(UART, g_vt100_MP_TEXT8, get_data_length(g_vt100_MP_TEXT8));

	UART_WriteBlocking(UART, g_vt100_MP_POS9 , get_data_length(g_vt100_MP_POS9));

}

void display_read_hr(UART_Type *UART)
{
	UART_WriteBlocking(UART, g_vt100_clear_screen, get_data_length(g_vt100_clear_screen));
	UART_WriteBlocking(UART,g_vt100_change_font , get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART, g_vt100_UpdateHour_POS1, get_data_length(g_vt100_UpdateHour_POS1));
	UART_WriteBlocking(UART, g_vt100_UpdateHour_TEXT1, get_data_length(g_vt100_UpdateHour_TEXT1));
	UART_WriteBlocking(UART, g_vt100_UpdateHour_POS2, get_data_length(g_vt100_UpdateHour_POS2));
	UART_WriteBlocking(UART, time_FINAL, get_data_length(time_FINAL));

}

void display_config_hr(UART_Type *UART)
{
	UART_WriteBlocking(UART, g_vt100_clear_screen, get_data_length(g_vt100_clear_screen));
	UART_WriteBlocking(UART,g_vt100_change_font , get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART, g_vt100_ConfigHr_POS1, get_data_length(g_vt100_ConfigHr_POS1));
	UART_WriteBlocking(UART, g_vt100_ConfigHr_TEXT1, get_data_length(g_vt100_ConfigHr_TEXT1));
	UART_WriteBlocking(UART, g_vt100_ConfigHr_POS2, get_data_length(g_vt100_ConfigHr_POS2));
}

void display_config_date(UART_Type *UART)
{
	UART_WriteBlocking(UART, g_vt100_clear_screen,     get_data_length(g_vt100_clear_screen));
	UART_WriteBlocking(UART,g_vt100_change_font ,      get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART,g_vt100_ConfigDate_POS1 ,  get_data_length(g_vt100_ConfigDate_POS1));
	UART_WriteBlocking(UART,g_vt100_ConfigDate_TEXT1 , get_data_length(g_vt100_ConfigDate_TEXT1));
	UART_WriteBlocking(UART,g_vt100_ConfigDate_POS2 ,  get_data_length(g_vt100_ConfigDate_POS2));
}

void display_read_temp(UART_Type *UART)
{
	UART_WriteBlocking(UART, g_vt100_clear_screen,   get_data_length(g_vt100_clear_screen));
	UART_WriteBlocking(UART,g_vt100_change_font ,    get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART,g_vt100_ReadTemp_POS1 ,  get_data_length(g_vt100_ReadTemp_POS1));
	UART_WriteBlocking(UART,g_vt100_ReadTemp_TEXT1 , get_data_length(g_vt100_ReadTemp_TEXT1));
	UART_WriteBlocking(UART,g_vt100_ReadTemp_POS2 ,  get_data_length(g_vt100_ReadTemp_POS2));
	UART_WriteBlocking(UART,temp_FINAL ,  get_data_length(temp_FINAL));

}

void display_read_hum(UART_Type *UART)
{
	UART_WriteBlocking(UART,g_vt100_clear_screen,   get_data_length(g_vt100_clear_screen));
	UART_WriteBlocking(UART,g_vt100_change_font ,   get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART,g_vt100_ReadHum_POS1 ,  get_data_length(g_vt100_ReadHum_POS1));
	UART_WriteBlocking(UART,g_vt100_ReadHum_TEXT1 , get_data_length(g_vt100_ReadHum_TEXT1));
	UART_WriteBlocking(UART,g_vt100_ReadHum_POS2 ,  get_data_length(g_vt100_ReadHum_POS2));
	UART_WriteBlocking(UART,hum_FINAL ,             get_data_length(hum_FINAL));

}

void display_date(UART_Type *UART)
{
	UART_WriteBlocking(UART, g_vt100_clear_screen,  get_data_length(g_vt100_clear_screen));
	UART_WriteBlocking(UART,g_vt100_change_font ,   get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART, g_vt100_ReadDate_POS1, get_data_length(g_vt100_ReadDate_POS1));
	UART_WriteBlocking(UART, g_vt100_ReadDate_TEXT1,get_data_length(g_vt100_ReadDate_TEXT1));
	UART_WriteBlocking(UART, g_vt100_ReadDate_POS2, get_data_length(g_vt100_ReadDate_POS2));
	UART_WriteBlocking(UART, date_FINAL,            get_data_length(date_FINAL));
	UART_WriteBlocking(UART, date_FINAL2,           get_data_length(date_FINAL2));
}

void display_new_hr_UART0(void)
{
	UART_WriteBlocking(UART0, g_vt100_UpdateHour_POS2, get_data_length(g_vt100_UpdateHour_POS2));
	UART_WriteBlocking(UART0, time_FINAL, get_data_length(time_FINAL));
}

void display_new_hr_UART4(void)
{
	UART_WriteBlocking(UART4, g_vt100_UpdateHour_POS2, get_data_length(g_vt100_UpdateHour_POS2));
	UART_WriteBlocking(UART4, time_FINAL, get_data_length(time_FINAL));
}

void clear_screen(UART_Type *UART)
{
	UART_WriteBlocking(UART, g_vt100_clear_screen,  get_data_length(g_vt100_clear_screen));
}

void resource_not_available(UART_Type *UART)
{
	UART_WriteBlocking(UART,g_vt100_change_font , get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART, g_vt100_Resource_POS1, get_data_length(g_vt100_Resource_POS1));
	UART_WriteBlocking(UART, g_vt100_Resource_TEXT1, get_data_length(g_vt100_Resource_TEXT1));
	UART_WriteBlocking(UART, g_vt100_Resource_POS2, get_data_length(g_vt100_Resource_POS2));
	set_UART_flag_false(UART);
}



uint8_t get_data_length(uint8_t *array)
{
    uint8_t i = 0;
    while(array[i] != '\0')
    {
        i++;
    }
    return i + NULL_CHARACTER;
}

void reset_terminal(void)
{
	UART_WriteBlocking(UART0, g_vt100_reset, get_data_length(g_vt100_reset));
	UART_WriteBlocking(UART4, g_vt100_reset, get_data_length(g_vt100_reset));
}

void change_display_attributes_terminal(void)
{
	UART_WriteBlocking(UART0,g_vt100_change_font , get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART4,g_vt100_change_font , get_data_length(g_vt100_change_font));
}

void I2C_device_fail_communication(UART_Type *UART)
{
	UART_WriteBlocking(UART, g_vt100_clear_screen,  get_data_length(g_vt100_clear_screen));
	UART_WriteBlocking(UART,g_vt100_change_font ,   get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART, g_vt100_I2C_Fail_POS1,  get_data_length(g_vt100_I2C_Fail_POS1));
	UART_WriteBlocking(UART,g_vt100_I2C_Fail_TEXT1 ,   get_data_length(g_vt100_I2C_Fail_TEXT1));
	UART_WriteBlocking(UART,g_vt100_I2C_Fail_POS2 ,   get_data_length(g_vt100_I2C_Fail_POS2));
}

void confirmation_CONFIG_HR(UART_Type *UART)
{
	UART_WriteBlocking(UART,g_vt100_change_font ,   get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART,g_vt100_Confirmation_HR_POS1 ,   get_data_length(g_vt100_Confirmation_HR_POS1));
	UART_WriteBlocking(UART,g_vt100_Confirmation_HR_TEXT1 ,   get_data_length(g_vt100_Confirmation_HR_TEXT1));
	UART_WriteBlocking(UART,g_vt100_Confirmation_HR_POS2 ,   get_data_length(g_vt100_Confirmation_HR_POS2));
}

void confirmation_CONFIG_DATE(UART_Type *UART)
{
	UART_WriteBlocking(UART,g_vt100_change_font ,   get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART,g_vt100_Confirmation_DATE_POS1 ,   get_data_length(g_vt100_Confirmation_DATE_POS1));
	UART_WriteBlocking(UART,g_vt100_Confirmation_DATE_TEXT1 ,   get_data_length(g_vt100_Confirmation_DATE_TEXT1));
	UART_WriteBlocking(UART,g_vt100_Confirmation_DATE_POS2 ,   get_data_length(g_vt100_Confirmation_DATE_POS2));
}

void display_active_log(UART_Type *UART)
{
	UART_WriteBlocking(UART, g_vt100_clear_screen,  get_data_length(g_vt100_clear_screen));
	UART_WriteBlocking(UART,g_vt100_change_font ,   get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART,g_vt100_Confirmation_LOG_POS1 ,   get_data_length(g_vt100_Confirmation_LOG_POS1));
	UART_WriteBlocking(UART,g_vt100_Confirmation_LOG_TEXT1 ,   get_data_length(g_vt100_Confirmation_LOG_TEXT1));
	UART_WriteBlocking(UART,g_vt100_Confirmation_LOG_POS2 ,   get_data_length(g_vt100_Confirmation_LOG_POS2));
}

void display_active_log_time(UART_Type *UART)
{
	UART_WriteBlocking(UART, g_vt100_clear_screen,  get_data_length(g_vt100_clear_screen));
	UART_WriteBlocking(UART,g_vt100_change_font ,   get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART,g_vt100_TimeCapture_LOG_POS1 ,   get_data_length(g_vt100_TimeCapture_LOG_POS1));
	UART_WriteBlocking(UART,g_vt100_TimeCapture_LOG_TEXT1 ,   get_data_length(g_vt100_TimeCapture_LOG_TEXT1));
	UART_WriteBlocking(UART,g_vt100_TimeCapture_LOG_POS2 ,   get_data_length(g_vt100_TimeCapture_LOG_POS2));
}

void display_active_log_capture(UART_Type *UART)
{
	UART_WriteBlocking(UART, g_vt100_clear_screen,  get_data_length(g_vt100_clear_screen));
	UART_WriteBlocking(UART,g_vt100_change_font ,   get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART,g_vt100_quantityCapture_LOG_POS1 ,   get_data_length(g_vt100_quantityCapture_LOG_POS1));
	UART_WriteBlocking(UART,g_vt100_quantityCapture_LOG_TEXT1 ,   get_data_length(g_vt100_quantityCapture_LOG_TEXT1));
	UART_WriteBlocking(UART,g_vt100_quantityCapture_LOG_POS2 ,   get_data_length(g_vt100_quantityCapture_LOG_POS2));
}

void confirmation_ACTIVATION_LOG(UART_Type *UART)
{
	UART_WriteBlocking(UART, g_vt100_clear_screen,  get_data_length(g_vt100_clear_screen));
	UART_WriteBlocking(UART,g_vt100_change_font ,   get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART,g_vt100_StartingCapture_LOG_POS1 ,   get_data_length(g_vt100_StartingCapture_LOG_POS1));
	UART_WriteBlocking(UART,g_vt100_StartingCapture_LOG_TEXT1 ,   get_data_length(g_vt100_StartingCapture_LOG_TEXT1));
	UART_WriteBlocking(UART,g_vt100_StartingCapture_LOG_POS2 ,   get_data_length(g_vt100_StartingCapture_LOG_POS2));
}

void display_read_log(UART_Type *UART)
{
	UART_WriteBlocking(UART, g_vt100_clear_screen,  get_data_length(g_vt100_clear_screen));
	UART_WriteBlocking(UART,g_vt100_change_font ,   get_data_length(g_vt100_change_font));
	UART_WriteBlocking(UART,g_vt100_Read_LOG_POS1 ,   get_data_length(g_vt100_Read_LOG_POS1));
	UART_WriteBlocking(UART,g_vt100_Read_LOG_TEXT1 ,   get_data_length(g_vt100_Read_LOG_TEXT1));
	UART_WriteBlocking(UART,g_vt100_Read_LOG_POS2 ,   get_data_length(g_vt100_Read_LOG_POS2));
}

void display_read_log_from_memory(UART_Type *UART)
{
	UART_WriteBlocking(UART,g_vt100_change_font ,   get_data_length(g_vt100_change_font));

	UART_WriteBlocking(UART,DATA_FINAL_TEMP ,   get_data_length(DATA_FINAL_TEMP));
	UART_WriteBlocking(UART,DATA_FINAL_HUM ,   get_data_length(DATA_FINAL_HUM));
	UART_WriteBlocking(UART,DATA_FINAL_HRS ,   get_data_length(DATA_FINAL_HRS));
	UART_WriteBlocking(UART,DATA_FINAL_DATE ,   get_data_length(DATA_FINAL_DATE));

}

void reading_log_repositioning(UART_Type *UART)
{
	UART_WriteBlocking(UART,g_vt100_Read_LOG_POS1 ,   get_data_length(g_vt100_Read_LOG_POS1));
}

void move_down_cursor(UART_Type *UART)
{
	UART_WriteBlocking(UART,g_vt100_down_cursor ,   get_data_length(g_vt100_down_cursor));
}

void update_hr_time(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	//Get the Teen of Hours value on the high part
	time_FINAL[ZERO] =  ((hours >> bit_4) & BITMASK_1BIT) + ASCII_OFFSET;
	//Get the Hours value on the low part
	time_FINAL[ONE] =  (hours & BITMASK_4BIT) + ASCII_OFFSET;
	//Get the Teen of Minutes value on the high part
	time_FINAL[THREE] =  ((minutes >> bit_4) & BITMASK_4BIT) + ASCII_OFFSET;//
	//Get the Minutes value on the low part
	time_FINAL[FOUR] =  (minutes & BITMASK_4BIT) + ASCII_OFFSET;//
	//Get the Teen of Seconds value on the high part
	time_FINAL[SIX] =  ((seconds >> bit_4) & BITMASK_4BIT) + ASCII_OFFSET;//
	//Get the Seconds value on the low part
	time_FINAL[SEVEN] =  (seconds & BITMASK_4BIT) + ASCII_OFFSET;//

}


void update_date(uint8_t day, uint8_t month, uint8_t year)
{
	date_FINAL[ZERO] = ((day >> bit_4) & BITMASK_4BIT) + ASCII_OFFSET;
	//Get the Hours value on the low part
	date_FINAL[ONE] =  (day & BITMASK_4BIT) + ASCII_OFFSET;

	//Get the Teen of Minutes value on the high part
	date_FINAL[THREE] =  ((month >> bit_4) & BITMASK_4BIT) + ASCII_OFFSET;//
	//Get the Minutes value on the low part
	date_FINAL[FOUR] =  (month & BITMASK_4BIT) + ASCII_OFFSET;//
	//Get the Teen of Seconds value on the high part
	date_FINAL2[TWO] =  ((year >> bit_4) & BITMASK_4BIT) + ASCII_OFFSET;//
	//Get the Seconds value on the low part
	date_FINAL2[THREE] =  (year & BITMASK_4BIT) + ASCII_OFFSET;//
}

void update_hum(uint8_t hum)
{
	hum_FINAL[ZERO] = hum;
}

void update_temp(uint8_t temp_tens,uint8_t temp_units,uint8_t temp_first_decimal,uint8_t temp_second_decimal)
{
	temp_FINAL[ZERO] = temp_tens;

	temp_FINAL[ONE] = temp_units;

	temp_FINAL[THREE] = temp_first_decimal;

	temp_FINAL[FOUR] = temp_second_decimal;
}

void update_data_final(uint8_t temp_tens, uint8_t temp_units, uint8_t temp_first_decimal, uint8_t temp_second_decimal, uint8_t hum, uint8_t teen_hours, uint8_t hours, uint8_t teen_minutes, uint8_t minutes, uint8_t teen_seconds, uint8_t seconds, uint8_t teen_day, uint8_t day, uint8_t teen_month, uint8_t month, uint8_t teen_year, uint8_t year)
{
	DATA_FINAL_TEMP[12] = temp_tens;
    DATA_FINAL_TEMP[13] = temp_units;
    DATA_FINAL_TEMP[15] = temp_first_decimal;
    DATA_FINAL_TEMP[16] = temp_second_decimal;

	DATA_FINAL_HUM[8] = hum;

	DATA_FINAL_HRS[0] = teen_hours;
	DATA_FINAL_HRS[1] = hours;
	DATA_FINAL_HRS[3] = teen_minutes;
	DATA_FINAL_HRS[4] = minutes;
	DATA_FINAL_HRS[6] = teen_seconds;
	DATA_FINAL_HRS[7] = seconds;

	DATA_FINAL_DATE[0] = teen_day;
	DATA_FINAL_DATE[1] = day;
	DATA_FINAL_DATE[3] = teen_month;
	DATA_FINAL_DATE[4] = month;
	DATA_FINAL_DATE[8] = teen_year;
	DATA_FINAL_DATE[9] = year;
}

void update_data_final2(uint8_t temp_tens, uint8_t temp_units, uint8_t temp_first_decimal, uint8_t temp_second_decimal, uint8_t hum, uint8_t teen_hours, uint8_t hours, uint8_t teen_minutes, uint8_t minutes, uint8_t teen_seconds, uint8_t seconds, uint8_t teen_day, uint8_t day, uint8_t teen_month, uint8_t month, uint8_t teen_year, uint8_t year)
{
	DATA_FINAL_TEMP2[12] = temp_tens;
    DATA_FINAL_TEMP2[13] = temp_units;
    DATA_FINAL_TEMP2[15] = temp_first_decimal;
    DATA_FINAL_TEMP2[16] = temp_second_decimal;

	DATA_FINAL_HUM2[8] = hum;

	DATA_FINAL_HRS2[0] = teen_hours;
	DATA_FINAL_HRS2[1] = hours;
	DATA_FINAL_HRS2[3] = teen_minutes;
	DATA_FINAL_HRS2[4] = minutes;
	DATA_FINAL_HRS2[6] = teen_seconds;
	DATA_FINAL_HRS2[7] = seconds;

	DATA_FINAL_DATE2[0] = teen_day;
	DATA_FINAL_DATE2[1] = day;
	DATA_FINAL_DATE2[3] = teen_month;
	DATA_FINAL_DATE2[4] = month;
	DATA_FINAL_DATE2[8] = teen_year;
	DATA_FINAL_DATE2[9] = year;
}

void write_on_screen(UART_Type *UART)
{
	UART_WriteBlocking(UART, &g_mail_box_uart_0.mail_box, UART_DATA_LENGTH);
}

uint8_t get_data(uint8_t lugar, UART_Type *UART)
{

	if (UART0 == UART)
	{

		return g_received_data_UART0[lugar];

	}

	if(UART4 == UART)
	{

		return g_received_data_UART4[lugar];

	}
	return ZERO;

}

uint8_t get_data_log(uint8_t lugar, UART_Type *UART)
{
	if (UART0 == UART)
	{

		return g_received_data_UART0_log[lugar];

	}

	if(UART4 == UART)
	{

		return g_received_data_UART4_log[lugar];

	}
	return ZERO;
}

void reset_UART_buffer_values(UART_Type *UART)
{

	if (UART0 == UART)
	{
		g_received_data_UART0[PLACE0] = 0;
		g_received_data_UART0[PLACE1] = 0;
		g_data_place_UART0 = 0;
	}

	if(UART4 == UART)
	{
		g_received_data_UART4[PLACE0] = 0;
		g_received_data_UART4[PLACE1] = 0;
		g_data_place_UART4 = 0;
	}

}

void reset_UART_buffer_values_log(UART_Type *UART)
{
	if (UART0 == UART)
	{
		g_received_data_UART0_log[PLACE0] = 0;
		g_received_data_UART0_log[PLACE1] = 0;
		g_data_place_UART0_log = 0;
	}

	if(UART4 == UART)
	{
		g_received_data_UART4_log[PLACE0] = 0;
		g_received_data_UART4_log[PLACE1] = 0;
		g_data_place_UART4_log = 0;
	}

}


uint8_t get_UART_value(UART_Type *UART)
{
	if (UART0 == UART)
	{
		return UART_ReadByte(UART0);
	}

	if(UART4 == UART)
	{
		return UART_ReadByte(UART4);
	}

	return ZERO;
}

void set_UART_flag_false(UART_Type *UART)
{
	if (UART0 == UART)
	{
		g_mail_box_uart_0.flag = FALSE;
	}

	if(UART4 == UART)
	{
		g_mail_box_uart_4.flag = FALSE;
	}
}

uint8_t get_UART_value_direct(UART_Type *UART)
{
	if (UART0 == UART)
	{
		return UART_ReadByte(UART0);
	}

	if(UART4 == UART)
	{
		return UART_ReadByte(UART4);
	}
	return ZERO;
}

uint8_t get_UART0_flag(void)
{
    if (g_mail_box_uart_0.flag == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

uint8_t get_UART4_flag(void)
{
    if (g_mail_box_uart_4.flag == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
