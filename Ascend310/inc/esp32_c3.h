#ifndef __ESP32_C3_H_
#define __ESP32_C3_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include "pthread.h"
#include "uart.h"



#define ESP32(args) args"\r\n"


typedef struct TEsp32c3Tab
{
    int device_fd;

}Esp32c3;

char Esp32Init(Esp32c3* esp32c3, char* device);

char Esp32AtCmd(Esp32c3* esp32c3, char* cmd);

#endif //__ESP32_C3_H_