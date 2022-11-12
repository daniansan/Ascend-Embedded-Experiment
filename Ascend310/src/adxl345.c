#include "adxl345.h"
#define Pi 3.14

Adxl345Handler* adxl345;

char adxl345_Init()
{   
    int ret;
    adxl345 = (Adxl345Handler*)malloc(sizeof(Adxl345Handler));
    if(adxl345 == NULL)
    {
        printf("malloc Adxl345Handler error\n");
        return -1;
    }

    ret = Spi_Init(&(adxl345->spi),"/dev/spidev0.1","adxl345",SPI_MODE_3,8,2000000,436);
    if(ret)
    {
        printf("adxl345 device init error\n");
        return -1;
    }


    adxl345_write_Cmd(ADXL345_REG_POWER_CTL,0x08);   /*< Configure */
    adxl345_write_Cmd(ADXL345_REG_DATA_FORMAT,0x0b);    /*< Configure full_res=1 16g*/
    adxl345_write_Cmd(ADXL345_REG_BW_RATE,0x0F);    /* rate 3200Hz*/
    adxl345_write_Cmd(ADXL345_REG_INT_ENABLE,0x00);
    adxl345_write_Cmd(ADXL345_REG_INT_MAP,0x00);
    adxl345_write_Cmd(ADXL345_REG_FIFO_CTL,0x00);

    printf("adxl345 device init success\n");
    return  0;
}

char adxl345_write_Cmd(unsigned char cmd,unsigned char data)
{
    unsigned char buff[4];
    buff[0] = data;
    Spi_Write(adxl345->spi,cmd,buff,1);

    memset(buff,0,4);
    cmd = cmd|0x80;
    Spi_Read(adxl345->spi,cmd,buff,1);
    if(buff[0] != data )
    {
        printf("Write 0x%02x Error\n",cmd);
        return -1;
    }
    return 0;
}
char adxl345_read_Cmd(unsigned char cmd,unsigned char* data)
{
    unsigned char buff[4];
    memset(buff,0,4);
    cmd = cmd|0x80;
    Spi_Read(adxl345->spi,cmd,buff,1);
    memcpy(data,buff,1);
    return 0;
}

void adxl345_GetData(float* data)
{
        unsigned char acc[6];
    short xyz[3];
    float x,y,z;
    float Angle[3];

    adxl345_read_Cmd(ADXL345_REG_DATAX0,&acc[0]);
    adxl345_read_Cmd(ADXL345_REG_DATAX1,&acc[1]);
    adxl345_read_Cmd(ADXL345_REG_DATAY0,&acc[2]);
    adxl345_read_Cmd(ADXL345_REG_DATAY1,&acc[3]);
    adxl345_read_Cmd(ADXL345_REG_DATAZ0,&acc[4]);
    adxl345_read_Cmd(ADXL345_REG_DATAZ1,&acc[5]);
    
    xyz[0] = (short)(acc[0] + ((unsigned short)acc[1] << 8));

    xyz[1]  = (short)(acc[2] + ((unsigned short)acc[3] << 8));

    xyz[2]  = (short)(acc[4] + ((unsigned short)acc[5] << 8));

    x = (float)xyz[0]*0.0039;
    y = (float)xyz[1]*0.0039;
    z = (float)xyz[2]*0.0039;

    Angle[0] = (atan(x / sqrt(y*y + z * z))) * 180/Pi;
    Angle[1] = (atan(y / sqrt(x*x + z * z))) * 180/Pi;
    Angle[2] = (atan(z / sqrt(x*x + y * y))) * 180/Pi;
    

    memcpy(&data[0],&Angle[0],sizeof(float));
    memcpy(&data[1],&Angle[1],sizeof(float));
    memcpy(&data[2],&Angle[2],sizeof(float));

    usleep(500000);
}
void adxl345_Free()
{
    Spi_Free(adxl345->spi);
    free(adxl345);
}
