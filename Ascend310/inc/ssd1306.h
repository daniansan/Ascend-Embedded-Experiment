#ifndef __SSD1306_H_
#define __SSD1306_H_

#include "i2c.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

//颜色反转
#define NORMAL_COLOR        0
#define TURNOVER_COLOR      1
//显示颠倒
#define NORMAL_DISPLAY      0
#define TURNOVER_DISPLAY    1
//字体大小
typedef enum{
    size1206 = 12,
    size1608 = 16,
    size2412 = 24
}CHAR_SIZE;


#define OLEDADDR 0x3c           //从机地址
#define WRITECMD 0x00           //写命令
#define WRITEDATA 0x40          //写数据

typedef struct Ssd1306Tab
{
    I2CHandler* iic;        
    unsigned char oledBuffer[128][8];
    char slaveaddr;
    int init;
}Ssd1306Handler;


char oled_init();

void oled_clear();
void oled_showPicture(const unsigned char BMP[]);
void ColorTurn(unsigned char arg);
void DisplayTurn(unsigned char arg);
void ShowString(unsigned char x,unsigned char y, char *c,CHAR_SIZE size);
void ShowChar(unsigned char x,unsigned char y,unsigned char c,CHAR_SIZE size);
void DrawLine(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2);
void DrawCircle(unsigned char x,unsigned char y,unsigned char r);
void DrawPoint(unsigned char x,unsigned char y);
void ClearPoint(unsigned char x,unsigned char y);
void Refresh();

void oled_free(void);

#endif
