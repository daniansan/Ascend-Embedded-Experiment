#ifndef _DS1399U_H_
#define _DS1399U_H_

#include "i2c.h"
#include <assert.h>

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;


#define TOD_BASE_YEAR 1988

#define DS1339_I2C_ADDRESS 0x68
#define DS1339_I2C_BUS_DEVICE "/dev/i2c-1"

#define DS1339_TIME_SECONDS 0X00
#define DS1339_TIME_MINUTES 0X01
#define DS1339_TIME_HOUR    0X02
#define DS1339_TIME_WEEK    0X03
#define DS1339_TIME_DAY     0X04
#define DS1339_TIME_MONTH   0X05
#define DS1339_TIME_YEAR    0X06

#define DS1339_ADDR_CTRL 0x0E
#define DS1339_CTRL_EOres 0x80
#define DS1339_CTRL_BBSQI 0x20
#define DS1339_CTRL_RS2 0x10
#define DS1339_CTRL_RS1 0x08
#define DS1339_CTRL_INTCN 0x04
#define DS1339_CTRL_A2IE 0x02
#define DS1339_CTRL_A1IE 0x01

#define DS1339_CTRL_DEFAULT (0x00)

#define DS1339_ADDR_TIME 0x00
#define DS1339_ADDR_STATUS 0x0F
#define DS1339_STATUS_OSF 0x80
#define DS1339_STATUS_A2F 0x02
#define DS1339_STATUS_A1F 0x01

#define DS1339_STATUS_CLEAR (0x00)



typedef struct
{
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
	uint32_t ticks;
}rtems_time_of_day;

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
#define DS1339_HOURS_12_24_FLAG 0x40
#define DS1339_HOURS_AM_PM_FLAG_OR_20_HOURS 0x20
#define DS1339_HOURS_10_HOURS 0x10
    uint8_t weekday;
    uint8_t date;
    uint8_t month;
#define DS1339_MONTH_CENTURY 0x80
    uint8_t year;
}ds1339_time_t;

/* The longest write transmission is writing the time + one address bit */
#define DS1339_MAX_WRITE_SIZE   (sizeof(ds1339_time_t) + 1)


typedef struct Ds1399uHanderTab
{
    I2CHandler* iic;
    char slaveaddr ;
}Ds1399uHander;




char ds1399u_init();
int rtc_get_time(rtems_time_of_day *tod);
int rtc_set_time(const rtems_time_of_day *tod);

char ds1399u_close();
#endif
