#include "spi.h"

char Spi_Init(SpiHandler** spi,const char* devpath,const char* devname,unsigned char dev_spi_mode,unsigned char dev_spi_bits,unsigned int dev_spi_speed,int gpioNum)
{
    int ret;
    char buff[36];
    (*spi) = (SpiHandler*)malloc(sizeof(SpiHandler));
    if((*spi) == NULL)
    {
        printf("malloc I2CHandler error\n");
        return -1;
    }
    memset((*spi),0,sizeof(SpiHandler));

    (*spi)->DeviceName = (char*)malloc(strlen(devname));
    if((*spi)->DeviceName == NULL)
    {
        printf("malloc DeviceName error\n");
        free(*spi);
        return -1;
    }

    (*spi)->DevicePath = (char*)malloc(strlen(devpath));
    if((*spi)->DevicePath == NULL)
    {
        printf("malloc DevicePath error\n");
        free((*spi)->DeviceName);
        free(*spi);
        return -1;
    }

    strcpy((*spi)->DeviceName,devname);
    strcpy((*spi)->DevicePath,devpath);
    (*spi)->spi_bits = dev_spi_bits;
    (*spi)->spi_mode = dev_spi_mode;
    (*spi)->spi_speed = dev_spi_speed;
    (*spi)->init = 0;


    (*spi)->spifd = open((*spi)->DevicePath, O_RDWR);
    if ((*spi)->spifd  < 0)
        printf("can't open device\n");

    /* spi mode */
    ret = ioctl((*spi)->spifd, SPI_IOC_WR_MODE, &(*spi)->spi_mode);
    if (ret == -1)
        printf("can't set spi mode\n");

    ret = ioctl((*spi)->spifd, SPI_IOC_RD_MODE, &(*spi)->spi_mode);
    if (ret == -1)
        printf("can't get spi mode\n");

    /* bits per word */
    ret = ioctl((*spi)->spifd, SPI_IOC_WR_BITS_PER_WORD, &(*spi)->spi_bits);
    if (ret == -1)
        printf("can't set bits per word\n");

    ret = ioctl((*spi)->spifd, SPI_IOC_RD_BITS_PER_WORD, &(*spi)->spi_bits);
    if (ret == -1)
        printf("can't get bits per word\n");

    /* max speed hz */
    ret = ioctl((*spi)->spifd, SPI_IOC_WR_MAX_SPEED_HZ, &(*spi)->spi_speed);
    if (ret == -1)
        printf("can't set max speed hz\n");

    ret = ioctl((*spi)->spifd, SPI_IOC_RD_MAX_SPEED_HZ, &(*spi)->spi_speed);
    if (ret == -1)
        printf("can't get max speed hz\n");


    //write only 只写 只有先向export写入数字，/sys/class/gpio下才能生成gpioXXX文件

	(*spi)->spi_cs_fd = open("/sys/class/gpio/export", O_WRONLY);
	if ((*spi)->spi_cs_fd < 0)
	{
		puts("Cannot open GPIO to export it\n");
		exit(1);
	}
    sprintf(buff,"%d",gpioNum);
    write((*spi)->spi_cs_fd,buff,4);
    close((*spi)->spi_cs_fd);

    // Update the direction of the GPIO to be an output
    memset(buff,0,sizeof(36));
    sprintf(buff,"/sys/class/gpio/gpio%d/direction",gpioNum);
    (*spi)->spi_cs_fd = open(buff, O_RDWR);
	if ((*spi)->spi_cs_fd< 0)
	{
		puts("Cannot open GPIO direction it\n");
		exit(1);
	}
	write((*spi)->spi_cs_fd, "out", 4);
	close((*spi)->spi_cs_fd);


    memset(buff,0,sizeof(36));
    sprintf(buff,"/sys/class/gpio/gpio%d/value",gpioNum);
    (*spi)->spi_cs_fd = open(buff, O_RDWR);
    if ((*spi)->spi_cs_fd < 0)
	{
		puts("Cannot open GPIO value\n");
		exit(1);
	}
    write((*spi)->spi_cs_fd,"1", 2);


    printf("%s device spi init success\n",(*spi)->DeviceName);
    return 0;

}
static void Spi_CS_High(int gpioValuefd)
{
    write(gpioValuefd,"1", 2);
}
static void Spi_CS_Low(int gpioValuefd)
{
    write(gpioValuefd,"0", 2);
}

static unsigned char Spi_Transfers(int spifd,unsigned char *tx,unsigned char *rx,unsigned int len)
{
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = len,
        .delay_usecs = 0,
        .speed_hz = 0,
        .bits_per_word = 8,
        
    };
    int ret;
    ret = ioctl(spifd, SPI_IOC_MESSAGE(1), &tr); //SPI_IOC_MESSAGE(N),进行N次读写    //ret返回的是成功发送或者接受的字节数
    //printf("spi treasfer ret:%d\n",ret);
    if (ret != len)
    {
        printf("can't send spi message\n");
	
        return -1;
    }
    else
    {
        return 0;
    }
}

char Spi_Write(SpiHandler* spi ,unsigned char reg,unsigned char* buff,int len)
{
    unsigned char command[len+1];
    unsigned char result[len+1];
    
    memset(result,0,len+1);
    
    command[0] = reg;
    memcpy(&command[1],buff,len);
    
    Spi_CS_Low(spi->spi_cs_fd);
    Spi_Transfers(spi->spifd,command,result,len+1);
    Spi_CS_High(spi->spi_cs_fd);
    return 0;
}
char Spi_Read(SpiHandler* spi,unsigned char reg,unsigned char* buff,int len)
{
    unsigned char command[len+1];
    unsigned char result[len+1];
    
    command[0] = reg;
    memset(&command[1],0,len);
    memset(result,0,len+1);
    
    Spi_CS_Low(spi->spi_cs_fd);
    Spi_Transfers(spi->spifd,command,result,len+1);
    Spi_CS_High(spi->spi_cs_fd);
    
    memcpy(buff,result+1,len);
    return 0;
}
char Spi_Free(SpiHandler* spi)
{
    free(spi->DeviceName);
    free(spi->DevicePath);
    free(spi);
    return 0;
}