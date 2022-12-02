#ifndef __MS1100__
#define __MS1100__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#define MS1100SLAVE 0x90


int MS1100Init();
int MS1100Config();
int MS1100Read(void* rbuff,int len);


#endif