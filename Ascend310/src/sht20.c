#include "sht20.h"

Sht20Handler* sht20;

int Sht20Init(const char* devicepath,char slaveaddr)
{   
    int ret = -1;


    sht20 = (Sht20Handler*)malloc(sizeof(Sht20Handler));
    if(sht20 == NULL)
    {
        printf("malloc Sht20Handler error\n");
        exit(-1);
    }

    ret = IIC_Init(&(sht20->iic),"sht20",devicepath); 
    if(ret)
    {
        sht20->init = 0;
        printf("sht20 init error\n");
        return -1;
    }
    printf("Sht20 device init successful\n");
    sht20->slaveaddr  = slaveaddr;
    sht20->init = 1;
    return 0;
}

int Sht20GetData(float* data)
{
    int ret = -1;
    unsigned char temporiBuf[3]; //温度传感器
    unsigned char humioriBuf[3]; //湿度传感器
    ret = IIC_Read(sht20->iic,sht20->slaveaddr,TRIG_T_MEASUREMENT_HM,temporiBuf);
    if(ret)
    {
        printf("atlas_i2c_read sht20 fail!\n");
        return -1;
    }
    //解析温度数据
    temporiBuf[1] = temporiBuf[1] & 0xFC; 
    unsigned short temp_tempori = (temporiBuf[0]<<8)|temporiBuf[1];
    sht20->tempori = -46.58+(175.75*temp_tempori)/(1<<16);


    //读取温度传感器数据
    ret = IIC_Read(sht20->iic,sht20->slaveaddr,TRIG_RH_MEASUREMENT_HM,humioriBuf);
    if(!ret)
    if (ret != 0) {
        printf("atlas_i2c_read sht20 fail!\n");
        return -1;
    }

    //解析湿度数据
    humioriBuf[1] = humioriBuf[1] & 0xFC;
    unsigned short temp_humiori = (humioriBuf[0]<<8)|humioriBuf[1];
    sht20->humiori = -6.0+(125.0*temp_humiori)/(1<<16);

    memcpy(data,&(sht20->humiori),sizeof(float));
    memcpy(&data[1],&(sht20->tempori),sizeof(float));
    return 0;
}

int Sht20Close()
{
    IIC_Free(sht20->iic);
    sht20->init = 0;
    sht20->humiori = 0;
    sht20->tempori = 0;
    sht20->slaveaddr =0;
    return 0;
}
