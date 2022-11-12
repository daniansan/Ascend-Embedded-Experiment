#include "task.h"

void Led_Pwm_Test(void)
{
    int flag = 0;
    int time = 0; 
    Ledhandler led;
    Led_Init(500,&led);
    while(1)
    {
      time = 0;
      while(!flag)
      {
        Led_High(&led);
        usleep(time*30);
        Led_Low(&led);
        usleep((255-time)*30);
        if((time++)==255)
          flag = 1;
      }
      time = 0;
      while(flag)
      {
        Led_High(&led);
        usleep((255-time)*30);
        Led_Low(&led);
        usleep(time*30);
        if((time++)==255)
          flag = 0;
      }
    }
}

void Led_Test(void)
{
    int time = 1000000; 
    Ledhandler led;
    Led_Init(500,&led);
    while(1)
    {
        Led_Low(&led);
        usleep(time);
        Led_High(&led);
        usleep(time);
    }
}




static uint8_t ReadKeyS2Status(void)
{
    uint8_t res = 0;
    Keyhandler key;
    Key_Init(S2,&key);
    res =  Key_Status(&key);
    Key_close(&key);
    return res ;
}
static void* S2_pressDown_callback_thread(void* args)
{
    printf("############### S2 Key  PRESS_DOWN ###############\n");
    Led_Test();
    return 0;
    
}
static void CallBackS2_PressDown(void* args)
{
    pthread_t taskid;
    pthread_create(&taskid,0,S2_pressDown_callback_thread,NULL);
    //pthread_join(taskid,NULL);
}
void Key_Test()
{
    Button*  button = (Button*)malloc(sizeof(Button));
    memset(button,0,sizeof(Button));
    TPin_level  pin_levels2 = ReadKeyS2Status;

    button_init(button,pin_levels2,0,1);
    button_attach(button,PRESS_DOWN,CallBackS2_PressDown);
    button_start(button);
    while(1)
    {
        button_ticks();
        usleep(5000);//5ms
    }
    button_stop(button);
    free(button);
}




/**
* ESP32 Ble测试案例
*0x304 为客户端写服务
*0x306 为客户端读服务
*/
static void *sppread(void* arg)
{
    Esp32c3* temp;
    char buff[1024];
    temp = (Esp32c3*)arg;
    while (1)
    {
        memset(buff,0,1024);
        uart_read(temp->device_fd,buff);
        if(strlen(buff) != 0)
            printf("\r\n>>%s\n",buff);
        usleep(1000000);
    }
    return 0;
}
static void *sppwrite(void* arg)
{
    Esp32c3* temp;
    char buff[1024]; 
    temp = (Esp32c3*)arg;
    while(1)
    {
        printf("<<");scanf("%s",buff);
        uart_send(temp->device_fd,buff);
        usleep(1000000);
    }
    return 0;
}
static void *idle_handle(void* arg)
{
    Esp32c3* temp;
    temp = (Esp32c3*)arg;
    while(1)
    {
        uart_send(temp->device_fd,"+++");
        usleep(1000000);
    }
    return 0;
}
/**
* ESP32 Ble测试案例
*/
void Esp32_Ble_Test()
{
    //int res = 0;
    char buff[1024];
    pthread_t read_tid;
    pthread_t write_tid;

    Esp32c3* esp32c3ble = (Esp32c3*)malloc(sizeof(Esp32c3));
    memset(esp32c3ble,0,sizeof(Esp32c3));

    Esp32Init(esp32c3ble,"/dev/ttyAMA1");
    Esp32AtCmd(esp32c3ble,"+++");
    usleep(1000000);
    Esp32AtCmd(esp32c3ble,"AT+RESTORE");
    Esp32AtCmd(esp32c3ble,"AT+CIPCLOSE");
    Esp32AtCmd(esp32c3ble,"AT+RESTORE");

    usleep(5000000);
    
    Esp32AtCmd(esp32c3ble,"AT");            /*< 查询AT 指令是否正常工作*/

    Esp32AtCmd(esp32c3ble,"ATE0");          /*< 查询AT 关闭回显*/

    Esp32AtCmd(esp32c3ble,"AT+GMR");        /*< 查询AT 版本信息*/

    Esp32AtCmd(esp32c3ble,"AT+BlEINIT?");   /*< 查询 Bluetooth LE 是否初始化*/

    Esp32AtCmd(esp32c3ble,"AT+BLEINIT=2");   /*< 初始化Bluetooth LE 服务端*/

    Esp32AtCmd(esp32c3ble,"AT+BLEGATTSSRVCRE");     /*< ESP32 Bluetooth LE 服务端创建服务*/

    Esp32AtCmd(esp32c3ble,"AT+BLEGATTSSRVSTART");   /*< ESP32 Bluetooth LE 服务端开启服务*/
    
    Esp32AtCmd(esp32c3ble,"AT+BLEADDR?");           /*< ESP32 Bluetooth LE 服务器获取其 MAC 地址*/

    Esp32AtCmd(esp32c3ble,"AT+BLEADVDATA=\"0201060A09457370726573736966030302A0\"");    /*<设置 Bluetooth LE 广播数据*/

    Esp32AtCmd(esp32c3ble,"AT+BLEADVSTART");        /*< ESP32 Bluetooth LE 服务端开始广播*/

    printf("手机打开 nRF 调试助手，并打开 SCAN 开始扫描，找到 ESP32 Bluetooth LE 服务端的 MAC 地址，点\
击 CONNECT 进行连接\n");                      /*< ESP32 Bluetooth LE 创建连接*/

    Esp32AtCmd(esp32c3ble,"AT+BLEGATTSSRV?");       /*< ESP32 Bluetooth LE 服务端查询服务*/

    Esp32AtCmd(esp32c3ble,"AT+BLEGATTSCHAR?");      /*< ESP32 Bluetooth LE 服务端发现特征*/

    printf("在手机 nRF 调试助手客户端点击 UUID:0xA002 的 UnKnown Service\n");   /*< ESP32 Bluetooth LE 客户端发现服务*/

    printf("此时在手机 nRF 调试助手客户端的 UUID:0xA002 的 UnKnown Service 服务下一级选项中选择点击\
Properties为NOTIFY 或者 INDICATE 的服务特征的右侧按钮（这里 ESP-AT 默认 Properties 为 NOTIFY\
或者 INDICATE 的服务特征是 0xC305 和 0xC306),开始侦听Properties为NOTIFY 或者 INDICATE 的\
服务特征\n");                                       /*< 手机 nRF 调试助手客户端发现特征*/


    printf("如果已经连接,请输入yes\n");
    while (1)
    {
        scanf("%s",buff);
        printf("%s",buff);

        if(strstr(buff,"yes") == NULL)
            continue;
        else
            break;
        memset(buff,0,sizeof(1024));
        usleep(1000000);
    }

    usleep(5000000);

    Esp32AtCmd(esp32c3ble,"AT+BLESPPCFG=1,1,7,1,5");    /*< ESP32 Bluetooth LE 服务端配置 Bluetooth LE SPP*/

    usleep(5000000);

    Esp32AtCmd(esp32c3ble,"AT+BLESPP");                 /*< ESP32 Bluetooth LE 服务端使能 Bluetooth LE SPP*/
    
    pthread_create(&write_tid,NULL,sppwrite,esp32c3ble);
    pthread_create(&read_tid,NULL,sppread,esp32c3ble);
    pthread_join(read_tid,NULL);
}

/**
* ESP32 UDP测试案例
*/
void Esp32_UdpClient_Test()
{
    char buff[1024];
    Esp32c3* esp32c3ble = (Esp32c3*)malloc(sizeof(Esp32c3));
    memset(esp32c3ble,0,sizeof(Esp32c3));


    Esp32Init(esp32c3ble,"/dev/ttyAMA1");
    Esp32AtCmd(esp32c3ble,"+++");
    usleep(1000000);
    Esp32AtCmd(esp32c3ble,"AT+RESTORE");
    Esp32AtCmd(esp32c3ble,"AT+CIPCLOSE");
    Esp32AtCmd(esp32c3ble,"AT+RESTORE");
    usleep(5000000);

    Esp32AtCmd(esp32c3ble,"ATE0");//取消回显
    Esp32AtCmd(esp32c3ble,"AT");//查询是否正常工作
    Esp32AtCmd(esp32c3ble,"AT+GMR");//查询版本信息

    Esp32AtCmd(esp32c3ble,"AT+CWMODE=2"); //设置esp32为SoftAP
    Esp32AtCmd(esp32c3ble,"AT+CWSAP=\"ESP32_SOFTAP\",\"1234567890\",5,3");//设置ESP32热点账号和密码
    usleep(10000000);//10s
    printf("Wait PC connect...\n");

    memset(buff,0,sizeof(1024));
    printf("确认PC是否开启UDP服务器");
    printf("如果已经开启,请输入yes\n");
    while (1)
    {
        scanf("%s",buff);
        printf("%s",buff);

        if(strstr(buff,"yes") == NULL)
            continue;
        else
            break;
        memset(buff,0,sizeof(1024));
        usleep(1000000);
    }
    Esp32AtCmd(esp32c3ble,"AT+CIPSTART=\"UDP\",\"192.168.4.2\",8080,2233,0");//连接PC的UDP服务器 
    usleep(1000000);

    Esp32AtCmd(esp32c3ble,"AT+CIPMODE=1");//开启 UART Wi-Fi 传输模式
    usleep(1000000);

    Esp32AtCmd(esp32c3ble,"AT+CIPSEND");//开启透传

    printf("发生 AT+CIPCLOSE  关闭TCP连接\n ");
    printf("发生 AT+CIPMODE=0  退出 UART Wi-Fi 透传模式\n ");
    while (1)
    {
        
        printf("输入任意字符开启传输：");
        scanf("%s",buff);
        Esp32AtCmd(esp32c3ble,buff);
        memset(buff,0,sizeof(1024));
    }
}

//该地址为心知天气
char http_url[]="AT+HTTPCLIENT=2,0,\"https://api.seniverse.com/v3/weather/now.json?key=SZRaEMZNu4ieYZ3R3&location=hangzhou&language=zh-Hans&unit=c\",,,2\r\n";
void Esp32_HttpClient_Test()
{
    char buff1[1024];
    char buff2[1024];
    char* begin;
    char* end; 
    int i=0;
    char* path ;
    const char* json_ = buff2;
    cJSON *item,*localtion,*now,*time,*temp;
    Esp32c3* esp32c3ble = (Esp32c3*)malloc(sizeof(Esp32c3));
    memset(esp32c3ble,0,sizeof(Esp32c3));

    cJSON* message;

    Esp32Init(esp32c3ble,"/dev/ttyAMA1");
    Esp32AtCmd(esp32c3ble,"+++");
    usleep(1000000);
    Esp32AtCmd(esp32c3ble,"ATE0");//取消回显
    Esp32AtCmd(esp32c3ble,"AT");//查询是否正常工作
    Esp32AtCmd(esp32c3ble,"AT+GMR");//查询版本信息

    Esp32AtCmd(esp32c3ble,"AT+CWMODE=1");       /*设置wifi 模式为station*/
    Esp32AtCmd(esp32c3ble,"AT+CWJAP=\"abc\",\"17816376987\""); /*连接路由器  参数分别为手机热点账号和密码*/ 
    usleep(5000000);
    
    
    Esp32AtCmd(esp32c3ble, http_url);       /*发送http 请求*/
    usleep(2000000);
    memset(buff1,0,1024);
    uart_read(esp32c3ble->device_fd,buff1);
    begin = strchr(buff1,'{');
    end = strrchr(buff1,'}');

    while(begin != end)
    { 
        buff2[i++]=*begin; 
        begin++;
    }


    buff2[i] = '}';
    buff2[i+1] = '\0';

    //printf("%s\n",buff2);

    message = cJSON_Parse(json_);

    // printf("%s\n",cJSON_Print(message));

    //返回的Json数据格式

    // {
    //     "results":[
    //         {
    //             "location":{
    //                 "id":"WTMKQ069CCJ7",
    //                 "name":"杭州",
    //                 "country":"CN",
    //                 "path":"杭州,杭州,浙江,中国",
    //                 "timezone":"Asia/Shanghai",
    //                 "timezone_offset":"+08:00"
    //                 },
    //             "now":{
    //                 "text":"小雨",
    //                 "code":"13",
    //                 "temperature":"18"
    //                 },
    //             "last_update":"2022-05-10T16:50:02+08:00"
    //         }
    //      ]
    // }
    
    item = cJSON_GetObjectItem(message,"results");
    item = cJSON_GetArrayItem(item,0);

    localtion = cJSON_GetObjectItem(item,"location");
    temp = cJSON_GetObjectItem(localtion,"name");
    path = cJSON_GetStringValue(temp);
    printf("地点:%s\n",path);


    time = cJSON_GetObjectItem(item,"last_update");
    path = cJSON_GetStringValue(time);
    printf("时间:%s\n",path);


    now = cJSON_GetObjectItem(item,"now");
    temp = cJSON_GetObjectItem(now,"text");
    path = cJSON_GetStringValue(temp);
    printf("天气:%s\n",path);
    temp = cJSON_GetObjectItem(now,"temperature");
    path = cJSON_GetStringValue(temp);
    printf("气温:%s\n",path);


    cJSON_free(message);
}


void Adxl345_Test()
{
    float buff[3];
    adxl345_Init();

    while (1)
    {
        adxl345_GetData(buff);
        printf("AngleX:%f AngleY:%f AngleZ:%f\n",buff[0],buff[1],buff[2]);
    }
}





void Oled_Test(void)
{
    oled_init();
    while (1)
    {
        ColorTurn(TURNOVER_COLOR);//反色显示
        DisplayTurn(NORMAL_DISPLAY);//正常显示
 
        ShowString(0,0,"abcdefg",size1206);//显示ASCII字符    
        ShowString(0,12,"1234567",size1608);//显示ASCII字符    
        ShowString(0,28,"ABCD",size2412);//显示ASCII字符    
        DrawCircle(40,32,20);
        DrawLine(0,0,60,60);
        Refresh();
    }
    
}



void Sht20_Test(void)
{

    Sht20Init("/dev/i2c-1",0x40);
    float buff[2];
    while (1)
    {
        Sht20GetData(buff);
        printf("温度为:%f \t湿度为:%f\n",buff[0],buff[1]);
        usleep(2000000);
    }
     Sht20Close();
}




void Pca9557_Test(void)
{
    pca9557_init("/dev/i2c-1");
    pca9557_setnum(1,2,3,4);
    pca9557_show();
}

void Ds1399u_Test()
{
    rtems_time_of_day tod={
        .year=2022,
        .month=6,
        .day=30,
        .hour=12,
        .minute=30,
        .second=30,
        .ticks =1
    };
    //rtc_set_time(&tod);
    memset(&tod,0,sizeof(tod));
    rtc_get_time(&tod);
    printf("%d-%d-%d %d:%d:%d\n",tod.year,tod.month,tod.day,tod.hour,tod.minute,tod.second);

    while(1)
    {
        sleep(1);
        rtc_get_time(&tod);
        printf("%d-%d-%d %d:%d:%d\n",tod.year,tod.month,tod.day,tod.hour,tod.minute,tod.second);
    }
}