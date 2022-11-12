#ifndef __SPI_H_
#define __SPI_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "sys/ioctl.h"
#include "linux/types.h"
#include "linux/spi/spidev.h"

typedef struct SpiHandlerTab
{
    unsigned char spi_mode;
    unsigned char spi_bits;
    unsigned char spi_speed;
    int spifd;
    int spi_cs_fd;

    int init;
   
    char* DeviceName;
    char* DevicePath;
}SpiHandler;


char Spi_Init(SpiHandler** spi,const char* devpath,const char* devname,unsigned char dev_spi_mode,unsigned char dev_spi_bits,unsigned int dev_spi_speed,int gpioNum);
char Spi_Write(SpiHandler* spi ,unsigned char reg,unsigned char* buff,int len);
char Spi_Read(SpiHandler* spi,unsigned char reg,unsigned char* buff,int len);
char Spi_Free(SpiHandler* spi);

#endif