#ifdef __cplusplus
extern "C"{
#endif

#include "task.h"
#include "unistd.h" 

#ifdef __cplusplus
}
#endif

#include "yolov3.h"



int main(int argc,char* argv[])
{
    char cmd[1024]={0};
    rtems_time_of_day tod={0};
    rtc_get_time(&tod);
    sprintf(cmd,"date -s \"%d-%d-%d %d:%d:%d\"",tod.year,tod.month,tod.day,tod.hour,tod.minute,tod.second);
    system(cmd);
    
    if (argc == 1)
    {
        goto help;
    }
    else if(argc == 2)
    {
        if (strcmp("-h", argv[1]) == 0)
        {
            help:
            printf("\033[0;33;34m-h                          实验内容 \033[m \n");
            printf("\033[0;33;34m-t Led_Test                 Led 控制实验 \033[m \r\n");
            printf("\033[0;33;34m-t Led_Pwm_Test             呼吸灯实验  \033[m \r\n");
            printf("\033[0;33;34m-t Key_Test                 按键实验   \033[m \r\n");
            printf("\033[0;33;34m-t Ble_Test                 Uart驱动和Esp32C3 低功耗蓝牙实验 \033[m \r\n");
            printf("\033[0;33;34m-t UdpClient_Test           Uart驱动和Esp32C3 UDP客户端实验 \033[m \r\n");
            printf("\033[0;33;34m-t Http_Test                Uart驱动和Esp32C3 Http实验 \033[m  \r\n");
            printf("\033[0;33;34m-t Spi_Test                 Spi驱动和Adxl345三轴加速度计实验 \033[m \r\n");
            printf("\033[0;33;34m-t Oled_Test                I2C驱动和Oled屏实验 \033[m \r\n");
            printf("\033[0;33;34m-t Sht20_Test               I2C驱动和温度传感器实验 \033[m\r\n");
            printf("\033[0;33;34m-t Pca9557_Test             I2C驱动和数码管实验 \033[m\r\n");   
            printf("\033[0;33;34m-t Ds1339u_Test             I2C驱动和rtc时钟实验 \033[m\r\n"); 
            printf("\033[0;33;34m-t V4L2_Test                v4l2摄像头实验 \033[m\r\n"); 
            printf("\033[0;33;34m-t Yolov3_Test              Yolov3扩展实验 \033[m\r\n");   
        }
    }
    else if (argc == 3)
    {
        if(strcmp("-t",argv[1]) == 0)
        {
            if(strcmp("Led_Test",argv[2]) == 0)
            {
                printf("This is Led Test\n");
                Led_Test();
            }
            else if(strcmp("Led_Pwm_Test",argv[2]) == 0)
            {
            printf("This is Led Pwn Test\n");
                Led_Pwm_Test();
            }
            else if(strcmp("Key_Test",argv[2]) == 0)
            {
                printf("This is Key Test\n");
                printf("Then Press S2 Key\n");
                Key_Test();
            }
            else if(strcmp("Ble_Test",argv[2]) == 0)
            {
                printf("This is Esp32 Ble Test\n");
                Esp32_Ble_Test();
            }
            else if(strcmp("Http_Test",argv[2]) == 0)
            {
                printf("This is Esp32 Http Test\n");
                Esp32_HttpClient_Test();
            }
            else if(strcmp("UdpClient_Test",argv[2]) == 0)
            {
                printf("This is Esp32 UdpClient Test\n");
                Esp32_UdpClient_Test();
            }
            else if(strcmp("Spi_Test",argv[2]) == 0)
            {
                printf("This is Spi Test \n");
                Adxl345_Test();
            }
            else if(strcmp("Oled_Test",argv[2]) == 0)
            {
                printf("This is Oled Test \n");
                Oled_Test();
            }
            else if(strcmp("Pca9557_Test",argv[2]) == 0)
            {
                printf("This is Pca9557 Test \n");
                Pca9557_Test();
            }
            else if(strcmp("Sht20_Test",argv[2]) == 0)
            {
                printf("This is Sht20 Test \n");
                Sht20_Test();
            }
            else if(strcmp("Ds1339u_Test",argv[2]) == 0)
            {
                printf("This is RTC Test \n");
                Ds1399u_Test();
            }
            else if(strcmp("Yolov3_Test",argv[2]) == 0)
            {
                printf("This is Yolov3 Tes \n");
                Yolov3_Test();
            }

            else if(strcmp("V4L2_Test",argv[2]) == 0)
            {
                printf("This is V4L2 Test \n");
                v4l2_test();
            }
        }
        if(strcmp("-test",argv[1]) == 0)
        {
            if(strcmp("xxxx",argv[2]) == 0)
            {
                /*Led 测试实验1*/
            }
            if(strcmp("xxxx",argv[2]) == 0)
            {
                /*Led 测试实验2*/
            }
        }
    }
}