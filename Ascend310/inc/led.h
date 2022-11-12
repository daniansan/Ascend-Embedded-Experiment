#ifndef __LED_H_
#define __LED_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fcntl.h"
#include "unistd.h"


typedef struct TLedhandlerTab
{
    int fd;
    char direction[5];
    int gpioNum;
}Ledhandler;



void Led_Init(int gpioNum,Ledhandler* ledhandler);
void Led_Low(Ledhandler* ledhandler);
void Led_High(Ledhandler* ledhandler);


#endif