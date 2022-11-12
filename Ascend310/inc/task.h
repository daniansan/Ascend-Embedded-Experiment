#ifndef __TASK_H_
#define __TASK_H_

#include "key.h"
#include "led.h"
#include "esp32_c3.h"
#include "adxl345.h"
#include "ssd1306.h"
#include "sht20.h"
#include "pca9557.h"
#include "cJSON.h"
#include "ds1339u.h"
#include "v4l2.h"
typedef uint8_t(*TPin_level)();


void Led_Test(void);
void Led_Pwm_Test(void);
void Key_Test(void);
void Esp32_Ble_Test(void);
void Esp32_UdpClient_Test(void);
void Esp32_HttpClient_Test(void);
void Adxl345_Test(void);
void Oled_Test(void);
void Sht20_Test(void);
void Pca9557_Test(void);
void Ds1399u_Test(void);



#endif