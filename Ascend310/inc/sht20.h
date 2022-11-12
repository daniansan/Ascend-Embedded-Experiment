#ifndef _SHT20_H_
#define _SHT20_H_
#include "i2c.h"

#define SHT20SLAVEADDR 0x40

//SHT2x基本指令集
#define TRIG_T_MEASUREMENT_HM    0xE3//主机模式触发温度测量
#define TRIG_RH_MEASUREMENT_HM   0xE5//主机模式触发湿度测量
#define TRIG_T_MEASUREMENT_POLL  0xF3//非主机模式触发温度测量
#define TRIG_RH_MEASUREMENT_POLL 0xF5//非主机模式触发湿度测量
#define USER_REG_W               0xE6//写用户寄存器
#define USER_REG_R               0xE7//读用户寄存器
#define SOFT_RESET               0xFE//软复位

typedef struct Sht20Tab
{
    I2CHandler* iic;            /*< IIC句柄*/
    char slaveaddr ;            /*< 设备地址*/
    float tempori;              /*< 温度数据*/
    float humiori;              /*< 湿度数据*/
    int init;                   /*< 是否初始化*/
    
}Sht20Handler;

/**
 * devicepath: 设备节点地址  /dev/i2c-1
 * slaveaddr: 设备IICd地址   0x40
 */ 
int Sht20Init(const char* devicepath,char slaveaddr);
/**
 * data : 返回的温湿度数据
 */
int Sht20GetData(float* data);
/**
 * 关闭设备，释放资源
 */ 
int Sht20Close();

#endif
