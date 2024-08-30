/*
 * RTC.h
 *
 *  Created on: 24 oct 2022
 *      Author: isaac
 */

#ifndef RTC_H_
#define RTC_H_

#include "stdint.h"
#include "MK64F12.h"

/* I2C source clock */
#define I2C_MASTER_CLK_SRC          I2C0_CLK_SRC
#define I2C_MASTER_CLK_FREQ         CLOCK_GetFreq(I2C0_CLK_SRC)
#define EXAMPLE_I2C_MASTER_BASEADDR I2C0

#define I2C_MASTER_SLAVE_ADDR_7BIT_RTC 0x68U //Data Read; Slave Address; 110 1000;
#define I2C_BAUDRATE               400000U
#define I2C_DATA_LENGTH            8U
#define DATE_LENGTH            6U
#define DATE_LENGTH2           4U
#define DATE_LENGTH3           6U
#define DATE_LENGTH4           2U

#define Year 6
#define Month 5
#define Day 4
#define Hour 2
#define Min 1
#define Sec 0
#define SQWE 7

#define INICIALIZATION_VALUE 6

struct time
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t month;
    uint8_t year;
};

struct number_time
{
    uint8_t teen_seconds;
    uint8_t seconds;
    uint8_t teen_minutes;
    uint8_t minutes;
    uint8_t teen_hours;
    uint8_t hours;
    uint8_t teen_day;
    uint8_t day;
    uint8_t teen_month;
    uint8_t month;
    uint8_t teen_year;
    uint8_t year;
};

void RTC_init(void);

int32_t write_RTC_hour(uint8_t hrs,uint8_t min,uint8_t seg);

int32_t write_RTC_date(uint8_t g_day,uint8_t g_month,uint8_t g_year);

uint8_t get_RTC_hr_values(uint8_t place);

void RTC_read_device(void);

struct number_time RTC_read_device_log(void);

void Read_Hr_State_flag (UART_Type *UART);

void reset_Read_Hr_State_flag (UART_Type *UART);

#endif /* RTC_H_ */
