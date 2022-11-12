#include "pca9557.h"



Pca9557Handler* pca9557;

static char NUM[10][2] = {
    {0x00,0x00},
    {0x3E,0xFB},  //1
    {0x2E,0x0D},  //2
    {0x2E,0x29},  //3
    {0x36,0x39},  //4
    {0x27,0x29},  //5
    {0x27,0x09},  //6
    {0x2E,0xFB},  //7
    {0x26,0x09},  //8
    {0x26,0x39}   //9
};

static void pca9557_wirte_cmd1(unsigned char reg, unsigned char value)
{
    IIC_Write(pca9557->iic,pca9557->slaveaddr1,reg,value);
}

static void pca9557_wirte_cmd2( unsigned char reg, unsigned char value)
{
    IIC_Write(pca9557->iic,pca9557->slaveaddr2,reg,value);
}

char pca9557_init(const char* devicepath)
{   
    int res;
    pca9557  = (Pca9557Handler*)malloc(sizeof(Pca9557Handler));
    if(pca9557 == NULL)
    {
        printf("malloc Pca9557Handler error\n");
        exit(-1);
    }
    memset(pca9557,0,sizeof(Pca9557Handler));
    res = IIC_Init(&(pca9557->iic),"pca9557",devicepath);
    if(res)
    {
        pca9557->init = 0;
        printf("pca9557 init error\n");
        return -1;
    }
    pca9557->init = 1;
    pca9557->slaveaddr1 = PCA9557_ADDR1;
    pca9557->slaveaddr2 = PCA9557_ADDR2;
    //设置管教输出方向，
    pca9557_wirte_cmd1(PCA9557_CONTROL_REG_3,0x00);
    pca9557_wirte_cmd2(PCA9557_CONTROL_REG_3,0x00);
    //设置管教极性反转 默认为11110000  即如这位是 1 则如管教设置1 则为0
    pca9557_wirte_cmd1(PCA9557_CONTROL_REG_2,0x00);
    pca9557_wirte_cmd2(PCA9557_CONTROL_REG_2,0x00);
    printf("pca9557 device init successful\n");
    return 0;

}


void static D1_ON_ONLY(char* cmd1,char* cmd2)
{
    *cmd1 = *cmd1 & 0x39;
    *cmd2 = *cmd2 & 0xFE;
}

void static D2_ON_ONLY(char* cmd1,char* cmd2)
{
    *cmd1 = *cmd1 & 0x1D;
    *cmd2 = *cmd2 & 0xFE;
}

void static D3_ON_ONLY(char* cmd1,char* cmd2)
{
    *cmd1 = *cmd1 & 0x1B;
    *cmd2 = *cmd2 & 0xFE;
}

void static D4_ON_ONLY(char* cmd1,char* cmd2)
{
    *cmd1 = *cmd1 & 0x19;
    *cmd2 = *cmd2 & 0xFF;
}

static void pca9557_setval(unsigned char oneval,unsigned char twoval)
{   
    pca9557_wirte_cmd1(PCA9557_CONTROL_REG_1,oneval);
    pca9557_wirte_cmd2(PCA9557_CONTROL_REG_1,twoval);  
}

void pca9557_setnum(int D1,int D2,int D3, int D4)
{
    pca9557->D1 = D1;
    pca9557->D2 = D2;
    pca9557->D3 = D3;
    pca9557->D4 = D4;
}

void pca9557_show()
{
    char cmd1,cmd2;
    int time = 6000;
    while(1)
    {

        cmd1 = NUM[pca9557->D4][0];
        cmd2 = NUM[pca9557->D4][1];
        D4_ON_ONLY(&cmd1,&cmd2);
        pca9557_setval(cmd1,cmd2);
        usleep(time);
        cmd1 = NUM[pca9557->D3][0];
        cmd2 = NUM[pca9557->D3][1];
        D3_ON_ONLY(&cmd1,&cmd2);
        pca9557_setval(cmd1,cmd2);
        usleep(time);
        cmd1 = NUM[pca9557->D2][0];
        cmd2 = NUM[pca9557->D2][1];
        D2_ON_ONLY(&cmd1,&cmd2);
        pca9557_setval(cmd1,cmd2);
        usleep(time);
        cmd1 = NUM[pca9557->D1][0];
        cmd2 = NUM[pca9557->D1][1];
        D1_ON_ONLY(&cmd1,&cmd2);
        pca9557_setval(cmd1,cmd2);
        usleep(time);
    }
}
char pca9557_close()
{
    memset(pca9557,0,sizeof(Pca9557Handler));
    IIC_Free(pca9557->iic);
    free(pca9557);
    return 0;
}   