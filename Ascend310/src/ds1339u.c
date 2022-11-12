
#include "ds1339u.h"

Ds1399uHander* ds1399u;

/* Functions for converting the fields */
static unsigned int get_seconds(ds1339_time_t *time) {
    //printf("0x%02x\t",time->seconds);
    uint8_t tens = time->seconds >> 4;
    uint8_t ones = time->seconds & 0x0F;
    return tens * 10 + ones;
}
static unsigned int get_minutes(ds1339_time_t *time) {
    //printf("0x%02x\t",time->minutes);
    uint8_t tens = time->minutes >> 4;
    uint8_t ones = time->minutes & 0x0F;
    return tens * 10 + ones;
}
static unsigned int get_hours(ds1339_time_t *time) {
    //printf("0x%02x\t",time->hours);
    uint8_t value = time->hours & 0x0F;

    if(time->hours & DS1339_HOURS_10_HOURS) {
        value += 10;
    }
    if(time->hours & DS1339_HOURS_AM_PM_FLAG_OR_20_HOURS) {
        if(time->hours & DS1339_HOURS_12_24_FLAG) {
        value += 12;
        } else {
        value += 20;
        }
    }

    return value;
}

static unsigned int get_day_of_month (ds1339_time_t *time) {
    //printf("0x%02x\t",time->date);
    uint8_t tens = time->date >> 4;
    uint8_t ones = time->date & 0x0F;
    return tens * 10 + ones;
}

static unsigned int get_month (ds1339_time_t *time) {
    //printf("0x%02x\t",time->month);
    uint8_t tens = (time->month >> 4) & 0x07;
    uint8_t ones = time->month & 0x0F;
    return tens * 10 + ones;
}

static unsigned int get_year (ds1339_time_t *time) {
    //printf("0x%02x\n",time->year);
    unsigned int year = 1900;
    year += (time->year >> 4) * 10;
    year += time->year & 0x0F;
    if(time->month & DS1339_MONTH_CENTURY) {
        year += 100;
    }
    if(year < TOD_BASE_YEAR) {
        year += 200;
    }
    return year;
}

static void set_time(ds1339_time_t *time,unsigned int second,unsigned int minute,
                        unsigned int hour,unsigned int day,unsigned int month,unsigned int year) 
{
    unsigned int tens;
    unsigned int ones;
    uint8_t century = 0;

    tens = second / 10;
    ones = second % 10;
    time->seconds = tens << 4 | ones;
    //printf("0x%02x\t",time->seconds);
    tens = minute / 10;
    ones = minute % 10;
    time->minutes = tens << 4 | ones;
    //printf("0x%02x\t",time->minutes);
    tens = hour / 10;
    ones = hour % 10;
    time->hours = tens << 4 | ones;
    //printf("0x%02x\t",time->hours);
    /* Weekday is not used. Therefore it can be set to an arbitrary valid value */
    time->weekday = 1;

    tens = day / 10;
    ones = day % 10;
    time->date = tens << 4 | ones;
    //printf("0x%02x\t",time->date);
    tens = month / 10;
    ones = month % 10;
    if(year >= 2000 && year < 2100) {
        century = DS1339_MONTH_CENTURY;
    }
    time->month = century | tens << 4 | ones;
    //printf("0x%02x\t",time->month);
    tens = (year % 100) / 10;
    ones = year % 10;
    time->year = tens << 4 | ones;
    //printf("0x%02x\n",time->year);
}
static int ds1399_open_file(int* fd)
{
    int res=0;
    *fd = open(DS1339_I2C_BUS_DEVICE,O_RDWR);
    if(*fd == -1)
    {
        printf("Can not open DS1399U\n");
        return -1;
    }
    res = ioctl(*fd,I2C_RETRIES,1);
    if(res == -1)
    {
        printf("DS1399U I2C Set I2C_RETRIES ERROR\n");
        return -1;
    }
    res = ioctl(*fd,I2C_TIMEOUT,10);
    if(res == -1)
    {
        printf("DS1399U I2C Set I2C_TIMEOUT ERROR\n");
        return -1;
    }
    return 0;
}


/* Read size bytes from ds1339 register address addr to buf. */
static int ds1339_read(uint8_t addr, void *buf, size_t size)
{
    int fd = -1;
    int res = 0;
    unsigned char regs = addr;
    struct i2c_rdwr_ioctl_data idata;
    struct i2c_msg msg[2];
    res = ds1399_open_file(&fd);
    if(res == -1)
    {
        printf("Openfile error\n");
        return -1;
    }
        
    memset(&idata,0,sizeof(idata));
    memset(&msg,0,sizeof(msg));

    idata.msgs = msg;
    idata.msgs[0].addr = DS1339_I2C_ADDRESS;
    idata.msgs[0].buf = &regs;
    idata.msgs[0].flags = 0;
    idata.msgs[0].len = 1;

    idata.msgs[1].addr = DS1339_I2C_ADDRESS;      /* slave address*/
    idata.msgs[1].buf = buf;        /*< pointer to msg data*/
    idata.msgs[1].flags = I2C_M_RD;  /*< 写为0, 读为I2C_M_RD*/
    idata.msgs[1].len = 1;           /*< msg length*/ 

    idata.nmsgs = 2;
    res = ioctl(fd,I2C_RDWR,&idata);
    if(res < 0)
    {
        printf("DS1399 read error res=%#x,errorno=%#x, %s \n",res,errno,strerror(errno));
        return -1;
    }
    close(fd);
    return 0;
}

/* Write size bytes from buf to ds1339 register address addr. */
static int ds1339_write(uint8_t addr, void *buf, size_t size)
{
    int fd = -1;
    int res = 0;
    struct i2c_rdwr_ioctl_data idata;
    struct i2c_msg msg[1];
    unsigned char local_buff[DS1339_MAX_WRITE_SIZE];
    int write_size = size + 1;
    assert(write_size <= DS1339_MAX_WRITE_SIZE);
    local_buff[0] = addr;
    memcpy(&local_buff[1], buf, size);

    res = ds1399_open_file(&fd);
    if(res == -1)
        return -1;
    memset(&idata,0,sizeof(idata));
    memset(&msg,0,sizeof(msg));

    idata.msgs = msg;
    idata.msgs->addr = DS1339_I2C_ADDRESS; ///* slave address*/
    idata.msgs->buf = local_buff;         /*< pointer to msg data*/
    idata.msgs->len = write_size;            /*< msg length*/ 
    idata.msgs->flags = 0;          /*< 写为0, 读为I2C_M_RD*/
    idata.nmsgs=1;
    res = ioctl(fd,I2C_RDWR,&idata);
    if(res < 0)
    {
        printf("DS1399 write error res=%#x,errorno=%#x, %s \n",res,errno,strerror(errno));
        return -1;
    }
    close(fd);
    return 0;
}

char ds1399u_init()
{
    int res = 0;
    uint8_t status = 0;

    /* Check RTC valid */
    res = ds1339_read(DS1339_ADDR_STATUS, &status, sizeof(status));
    assert(res == 0);
    if(status & DS1339_STATUS_OSF) {
        /* RTC has been stopped. Initialise it. */
        ds1339_time_t time;

        uint8_t write = DS1339_CTRL_DEFAULT;
        res = ds1339_write(DS1339_ADDR_CTRL, &write, sizeof(write));
        assert(res == 0);

        write = DS1339_STATUS_CLEAR;
        res = ds1339_write(DS1339_ADDR_STATUS, &write, sizeof(write));
        assert(res == 0);

        set_time(&time, 30, 25, 11,30, 6, 2022);
        res = ds1339_write(DS1339_ADDR_TIME, &time, sizeof(time));
        assert(res == 0);
    }
    printf("DS1399U Init Successful\n");
    return 0;
}

char ds1399u_close()
{
    return 0;
}

int rtc_get_time(rtems_time_of_day *tod)
{
    ds1339_time_t time;
    int res = 0;
    rtems_time_of_day temp_tod;

    ds1339_read(DS1339_TIME_SECONDS, &time.seconds, sizeof(time.seconds));
    ds1339_read(DS1339_TIME_MINUTES, &time.minutes, sizeof(time.minutes));
    ds1339_read(DS1339_TIME_HOUR, &time.hours, sizeof(time.hours));
    ds1339_read(DS1339_TIME_DAY, &time.date, sizeof(time.date));
    ds1339_read(DS1339_TIME_MONTH, &time.month, sizeof(time.month));
    ds1339_read(DS1339_TIME_YEAR, &time.year,sizeof(time.year));
    temp_tod.ticks = 0;
    temp_tod.second = get_seconds(&time);
    temp_tod.minute = get_minutes(&time);
    temp_tod.hour = get_hours(&time);
    temp_tod.day = get_day_of_month(&time);
    temp_tod.month = get_month(&time);
    temp_tod.year = get_year(&time);

    if(&temp_tod)
    {
        memcpy(tod, &temp_tod, sizeof(temp_tod));
    } else 
    {
        res = -1;
    }
    return res;
}
int rtc_set_time(const rtems_time_of_day *tod)
{
    ds1339_time_t time;

    set_time (&time,tod->second,tod->minute,tod->hour,tod->day,tod->month,tod->year);
    ds1339_write(DS1339_TIME_SECONDS, &time.seconds, sizeof(time.seconds));
    ds1339_write(DS1339_TIME_MINUTES, &time.minutes, sizeof(time.minutes));
    ds1339_write(DS1339_TIME_HOUR, &time.hours, sizeof(time.hours));
    ds1339_write(DS1339_TIME_DAY, &time.date, sizeof(time.date));
    ds1339_write(DS1339_TIME_MONTH, &time.month, sizeof(time.month));
    ds1339_write(DS1339_TIME_YEAR, &time.year,sizeof(time.year));
    
}
