#include "i2c.h"



char IIC_Init(I2CHandler** iic,const char* deviceName,const char* devicePath)
{
    *iic = (I2CHandler*)malloc(sizeof(I2CHandler));
    if(*iic == NULL)
    {
        printf("malloc I2CHandler error\n");
        return -1;
    }
    (*iic)->DeviceName = (char*)malloc(strlen(deviceName));
    if((*iic)->DeviceName == NULL)
    {
        printf("malloc DeviceName error\n");
        return -1;
    }

    (*iic)->DevicePath = (char*)malloc(strlen(devicePath));
    if((*iic)->DevicePath == NULL)
    {
        printf("malloc DevicePath error\n");
        return -1;
    }
    strcpy((*iic)->DeviceName,deviceName);
    strcpy((*iic)->DevicePath,devicePath);
    
    //printf("File:%s Line:%d %s     %s\n",__FILE__,__LINE__,iic->DeviceName,iic->DevicePath);
    (*iic)->init = 0;

    (*iic)->i2cfd = open((*iic)->DevicePath,O_RDWR);
    if((*iic)->i2cfd < 0)
    {
        printf("%s device can not open\n",(*iic)->DeviceName);
        free((*iic)->DeviceName);
        free((*iic)->DevicePath);
        free(*iic);
        return -1;
    }

     // 设置适配器收不到ACK时重试的次数 默认为1
    if(ioctl((*iic)->i2cfd,I2C_RETRIES,1) < 0)
    {   
        close((*iic)->i2cfd);
        (*iic)->i2cfd = 0;
        printf("%s set  retry faild\n",(*iic)->DeviceName);
        free((*iic)->DeviceName);
        free((*iic)->DevicePath);
        free(*iic);
        return -1;
    }
    // 设置超时时间 10ms作为一个单元  这里设置等待100ms ，后面要改进
    if(ioctl((*iic)->i2cfd,I2C_TIMEOUT,10) < 0)
    {
        close((*iic)->i2cfd);
        (*iic)->i2cfd = 0;
        printf("%s set  timeout faild\n",(*iic)->DeviceName);
        free((*iic)->DeviceName);
        free((*iic)->DevicePath);
        free(*iic);
        return -1;
    }
    (*iic)->init = 1;
    printf("%s iic init success\n",(*iic)->DeviceName);
    return 0;
}   

char IIC_Write(I2CHandler* iic,unsigned char slave, unsigned char reg, unsigned char value)
{
    int res = -1;
    if(!iic->init)
        return -1;
    
    memset(&iic->idata,0,sizeof(iic->idata));
    unsigned char buff[2] = {0};

    iic->idata.msgs = (struct i2c_msg *)malloc( sizeof(struct i2c_msg));
    if (iic->idata.msgs == NULL) {
        printf("Memory alloc error!");
        return -1;
    }

    buff[0] = reg;
    buff[1] = value;
    
    iic->idata.msgs->addr = slave;       /* slave address*/
    iic->idata.msgs->buf = buff;         /*< pointer to msg data*/
    iic->idata.msgs->len = 2;            /*< msg length*/ 
    iic->idata.msgs->flags = 0;          /*< 写为0, 读为I2C_M_RD*/


    iic->idata.nmsgs = 1;        /*< number of messages to exchange */

    res = ioctl(iic->i2cfd,I2C_RDWR,&(iic->idata));
    if(res < 0)
    {
        printf("%s write error res=%#x,errorno=%#x, %s \n",iic->DeviceName,res,errno,strerror(errno));
        free(iic->idata.msgs);
        return -1;
    }
    free(iic->idata.msgs);
    return 0;
}


char IIC_Read(I2CHandler* iic,unsigned char slave, unsigned char reg,unsigned char* buff)
{
    int res = -1;
    unsigned char regs[2] = {0};
    regs[0] = reg;
    regs[1] = reg;
    
    if(iic == NULL)
    {
        printf("File:%s line:%d ERROR\n",__FILE__,__LINE__);
        exit(-1);
    }
    iic->idata.msgs = (struct i2c_msg *)malloc(2*sizeof(struct i2c_msg));
    if (iic->idata.msgs == NULL) {
        printf("Memory alloc error!");
        return -1;
    }
    (iic->idata.msgs[0]).addr = slave;
    (iic->idata.msgs[0]).buf = regs;
    (iic->idata.msgs[0]).flags = 0;
    (iic->idata.msgs[0]).len = 1;

    (iic->idata.msgs[1]).addr = slave;      /* slave address*/
    (iic->idata.msgs[1]).buf = buff;        /*< pointer to msg data*/
    (iic->idata.msgs[1]).flags = I2C_M_RD;  /*< 写为0, 读为I2C_M_RD*/
    (iic->idata.msgs[1]).len = 1;           /*< msg length*/ 

    iic->idata.nmsgs=2;

    res = ioctl(iic->i2cfd,I2C_RDWR,&(iic->idata));
    if(res < 0)
    {
        printf("read data error,ret=%#x !\n", res);
        free(iic->idata.msgs);
        return -1;
    }

    free(iic->idata.msgs);
    return 0;
}

char IIC_Free(I2CHandler* iic)
{
    memset(iic,0,sizeof(I2CHandler));
    free(iic->DeviceName);
    free(iic->DevicePath);
    free(iic);
    return 0;
}