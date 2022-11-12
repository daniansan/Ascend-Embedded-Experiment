#include "key.h"

void Key_Init(KeyNum gpioNum,Keyhandler* key)
{
    switch (gpioNum)
    {
    case S2:
        {
            key->fd = open("/sys/class/gpio/gpio457/value",O_RDWR);
            if(key->fd < 0)
            {
                printf("Open /sys/class/gpio/gpio457/value Error\n");
            }
        }
        break;
    default:
        {
            key->fd = open("/sys/class/gpio/gpio456/value",O_RDWR);
            if(key->fd < 0)
            {
                printf("Open /sys/class/gpio/gpio457/value Error\n");
            }
        }
        break;
    }
}

char Key_Status(Keyhandler* key)
{
    int res;
    char  status[4]= {0};
    res = read(key->fd,&status,sizeof(status));
    if(res < 0)
    {
        printf("Read status error\n");
    }
    close(key->fd);
    if(strchr(status,'1') !=NULL )
        return 1;
    else return 0;   
}

void Key_close(Keyhandler* key)
{
    memset(key,0,sizeof(Keyhandler));
}