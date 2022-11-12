#include "esp32_c3.h"

char Esp32Init(Esp32c3* esp32c3, char* device)
{
    int res =-1;
    if(device == NULL)
        return -1;
    
    res = uart_open(&(esp32c3->device_fd),device);

    if(res == -1)
    {
        printf("device open error\n");
        return -1;
    }
    res = uart_set_option(esp32c3->device_fd,115200,8,'N',1);
    if(res == -1)
    {
        printf("set uart opention error\n");
        return -1;
    }
    return 0;
}


char Esp32AtCmd(Esp32c3* esp32c3,char* cmd)
{
    int len = strlen(cmd);
    char buff[1024];
    memset(buff,0,1024);

    char* atcmd = (char* )malloc(len+2);
    memset(atcmd,0,len+2);
    memcpy(atcmd,cmd,len);
    strcat(atcmd,"\r\n");

    printf("AT: %s",atcmd);

    uart_send(esp32c3->device_fd,atcmd);
    usleep(1000000);
    uart_read(esp32c3->device_fd, buff);
    printf("%s",buff);
    free(atcmd);
    return 0;
}

