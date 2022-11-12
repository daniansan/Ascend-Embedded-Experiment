#ifndef _PCA9557_H_
#define _PCA9557_H_

#include "i2c.h"

#define PCA9557_ADDR1       0x18
#define PCA9557_ADDR2       0x1c



/* 控制寄存器 （CMD）*/
#define PCA9557_CONTROL_REG_0 0x00	// Input Port Register           (R)    BXXXXXXXX (Default)
#define PCA9557_CONTROL_REG_1 0x01	// Output Port Register          (R/W)  B00000000
#define PCA9557_CONTROL_REG_2 0x02	// Polarity Inversion Register   (R/W)  B11110000
#define PCA9557_CONTROL_REG_3 0x03	// Configuration Register        (R/W)  B11111111


#define D1_ON(cmd1,cmd2)          (cmd1& 0x29)  //只开启D1
#define D2_ON(cmd1,cmd2)          (cmd1& 0x1C)   //只开启D1
#define D3_ON(cmd1,cmd2)          (cmd1& 0x1D)
#define D4_ON(cmd1,cmd2)          (cmd2& 0xFE)


/**
 *  1: 0x3E 0xFB
 *  2: 0x2E 0x0D
 *  3: 0x2E 0x29
 *  4: 0x36 0x39
 *  5: 0x27 0x29
 *  6: 0x27 0x09
 *  7: 0x2E 0xFB
 *  8: 0x26 0x09
 *  9: 0x26 0x39
 */


typedef struct Pca9557HandlerTab
{
    I2CHandler* iic;
    char slaveaddr1 ;
    char slaveaddr2 ;
    int init;
    int D1;
    int D2;
    int D3;
    int D4;
}Pca9557Handler;

char pca9557_init(const char* devicepath);
void pca9557_setnum(int D1,int D2,int D3, int D4);
void pca9557_show();
char pca9557_close(void);


#endif
