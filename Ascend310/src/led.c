#include "led.h"

void Led_Init(int gpioNum,Ledhandler* ledhandler)
{
    int tempfd; 
    char buff[100] = {0};

    strcpy(ledhandler->direction,"out");
    ledhandler->gpioNum = gpioNum;

    tempfd = open("/sys/class/gpio/export", O_WRONLY);
	if (tempfd < 0)
	{
		printf("Cannot open GPIO to export it\n");
		exit(1);
	}
    memset(buff,0,100);
    sprintf(buff,"%d",ledhandler->gpioNum);
    write(tempfd,buff,4);
    close(tempfd);

    memset(buff,0,100);
    sprintf(buff,"/sys/class/gpio/gpio%d/direction",ledhandler->gpioNum);
    tempfd = open(buff,O_RDWR);
    if (tempfd < 0)
	{
		printf("Cannot open GPIO direction it\n");
		exit(1);
	}
    write(tempfd, ledhandler->direction, 4);
    close(tempfd);

    memset(buff,0,100);
    sprintf(buff,"/sys/class/gpio/gpio%d/value",ledhandler->gpioNum);
    ledhandler->fd = open(buff,O_RDWR);
    if (tempfd < 0)
	{
		printf("Cannot open GPIO direction it\n");
		exit(1);
	}
    write(ledhandler->fd  ,"0", 2);
}

void Led_Low(Ledhandler* ledhandler)
{
    write(ledhandler->fd,"0",2);
}


void Led_High(Ledhandler* ledhandler)
{
    write(ledhandler->fd,"1",2);
}