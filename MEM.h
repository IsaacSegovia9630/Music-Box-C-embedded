
/*
 * MEM.h
 *
 *	configuración para el funcionamiento y uso de la memoria externa
 *  Created on: 26/10/2022
 *      Author: Tahirí
 */

#ifndef MEM_H_
#define MEM_H_

#include "fsl_i2c.h"
#include "PIT.h"
#include "MK64F12.h"
#include "NVIC.h"



#define I2C_MEM_MASTER_CLK_SRC          I2C0_CLK_SRC
#define I2C_MEM_MASTER_CLK_FREQ         CLOCK_GetFreq(I2C0_CLK_SRC)
#define I2C_MEM_MASTER_BASEADDR 		I2C0

#define MEM_MASTER_SLAVE_ADDR_7BIT	 0x50U
#define MEM_ADRESS_LENGTH    		 0x02
#define BUFF_MEM					 17U
#define I2C_MASTER_SLAVE_ADDR_7BIT   0x6FU

#define SUB_ADRESS_TEST   0x0U


void write_mem(uint8_t MSJ[], uint16_t mem_adress, uint16_t size);

void write_mem_log_init(UART_Type *UART, uint8_t time, uint8_t capture);

uint8_t mem_communication_test(void);

/*
 * lee lo que tiene guardado la memoria y me regresa un estado en falso o verdadero para verificar si leyó
 */
void read_mem(uint16_t mem_adress, uint16_t size, UART_Type *UART);



#endif /* MEM_H_ */
