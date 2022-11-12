
#ifndef __I2C_H_
#define __I2C_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

typedef struct I2CHandlerTab{
    char* DeviceName;
    char* DevicePath;
    int i2cfd;
    struct i2c_rdwr_ioctl_data idata;
    int init;       /*< 初始化标志*/
}I2CHandler;


char IIC_Init(I2CHandler** iic,const char* deviceName,const char* devicePath);
char IIC_Write(I2CHandler* iic,unsigned char slave, unsigned char reg, unsigned char value);
char IIC_Read(I2CHandler* iic,unsigned char slave, unsigned char reg,unsigned char* buff);
char IIC_Free(I2CHandler* iic);
#endif //__I2C_H_
