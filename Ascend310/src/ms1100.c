#include "ms1100.h"
#include "errno.h"


int ms1100fd;

int  MS1100Init()
{
    ms1100fd = open("/dev/i2c-1",O_RDWR);
    if(ms1100fd<0)
    {
        printf("[ERROR] Open ms1100 fd error\n");
        return -1;
    }

    if(ioctl(ms1100fd,I2C_TIMEOUT,2)<0)
    {
        printf("[ERROR] Ioctl ms1100 I2C_TIMEOUT error\n");
        close(ms1100fd);
        return -1;
    }

    if(ioctl(ms1100fd,I2C_RETRIES,1)<0)
    {
        printf("[ERROR] Ioctl ms1100 I2C_RETRIES error\n");
        close(ms1100fd);
        return -1;
    }
    return 0;
}


int MS1100Config()
{

    int res = -1;
    char buff[1]={0x0C};
    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg i2cmsg;

    i2cmsg.addr = (MS1100SLAVE>>1);
    i2cmsg.flags = 0;
    i2cmsg.buf = buff;
    i2cmsg.len = 1;

    data.msgs = &i2cmsg;
    data.nmsgs = 1;

    res = ioctl(ms1100fd,I2C_RDWR,&data);
    if(res < 0)
    {
        printf("[ERROR]%d Ioctl ms1100 I2C_RDWR with %s\n",__LINE__,strerror(errno));
        return -1;
    }
    return 0;
}

int MS1100Read(void* rbuff,int len)
{
    int res = -1;
    char buff[4];
    char tbuff[1]={0x00};
    struct i2c_rdwr_ioctl_data data;
	struct i2c_msg i2cmsg[2];

    i2cmsg[0].addr = (MS1100SLAVE>>1);
    i2cmsg[0].flags = 0;
    i2cmsg[0].len = 1;
    i2cmsg[1].buf = tbuff;

    i2cmsg[1].addr = (MS1100SLAVE>>1);
    i2cmsg[1].buf = buff;
    i2cmsg[1].flags = I2C_M_RD;
    i2cmsg[1].len = 3;

    data.msgs = i2cmsg;
    data.nmsgs = 2;

    res = ioctl(ms1100fd,I2C_RDWR,&data);
    if(res < 0)
    {
        printf("[ERROR]%d Ioctl ms1100 I2C_RDWR with %s\n",__LINE__,strerror(errno));
        return -1;
    }
    *rbuff = (buff[0]<<4)|buff[1];
    return 0;
}