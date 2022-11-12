#include "ssd1306.h"
#include "font.h"

Ssd1306Handler* ssd1306handler;



static void oled_write_cmd(unsigned char value)
{
    IIC_Write(ssd1306handler->iic,ssd1306handler->slaveaddr,WRITECMD,value);
}

static void oled_write_data(unsigned char value)
{
    IIC_Write(ssd1306handler->iic,ssd1306handler->slaveaddr,WRITEDATA,value);
}

char oled_init()
{
    int ret=0;
    ssd1306handler  = (Ssd1306Handler*)malloc(sizeof(Ssd1306Handler));
    if(ssd1306handler == NULL)
    {
        printf("malloc Ssd1306Handler error\n");
        exit(-1);
    }
    memset(ssd1306handler,0,sizeof(Ssd1306Handler));

    ret = IIC_Init(&(ssd1306handler->iic),"oled","/dev/i2c-1");
    if(ret)
    {
        ssd1306handler->init = 0;
        printf("oled init error\n");
        return -1;
    }
    ssd1306handler->init = 1;
    memset(ssd1306handler->oledBuffer,0,sizeof(ssd1306handler->oledBuffer));
    ssd1306handler->slaveaddr = OLEDADDR;

    oled_write_cmd(0xAE);
    oled_write_cmd(0x00);
    oled_write_cmd(0x10);
    oled_write_cmd(0x40);
    oled_write_cmd(0x81);
    oled_write_cmd(0xCF);
    oled_write_cmd(0xA1);

    oled_write_cmd(0xC8);
    oled_write_cmd(0xA6);
    oled_write_cmd(0xA8);
    oled_write_cmd(0x3f);
    oled_write_cmd(0xD3);
    oled_write_cmd(0x00);
    oled_write_cmd(0xd5);

    oled_write_cmd(0x80);
    oled_write_cmd(0xD9);
    oled_write_cmd(0xF1);
    oled_write_cmd(0xDA);
    oled_write_cmd(0x12);
    oled_write_cmd(0xDB);
    oled_write_cmd(0x40);
    
    oled_write_cmd(0x20);
    oled_write_cmd(0x02);
    oled_write_cmd(0x8D);
    oled_write_cmd(0x14);
    oled_write_cmd(0xA4);
    oled_write_cmd(0xA6);
    oled_write_cmd(0xAF);

    oled_clear();
    return 0;
}

void oled_clear()
{
     for(int i=0;i<8;i++)
    {
        oled_write_cmd(0xb0+i);
        oled_write_cmd(0x00);
        oled_write_cmd(0x10);
        for(int n=0;n<128;n++)
        {
            oled_write_data(0x00);
        }
    } 
}

void oled_showPicture(const unsigned char BMP[])
{
    int j = 0;;
    for(int y=0;y<8;y++)
    {
        oled_write_cmd(0xb0+y);
        oled_write_cmd(((0x00&0xf0)>>4)|0x10);
        oled_write_cmd((0x00&0x0f)|0x01);
        for (int x = 0; x < 128; x++)
        {
            oled_write_data(BMP[j]);
            j++;
        }
    }
}

void ColorTurn(unsigned char arg)
{
    if(arg == NORMAL_COLOR)
    {
        oled_write_cmd(0xA6);//正常显示
    }
    if(arg == TURNOVER_COLOR)
    {
        oled_write_cmd(0xA7);//反色显示
    }
}

void DisplayTurn(unsigned char arg)
{
    if(arg == NORMAL_DISPLAY)
    {
        oled_write_cmd(0xC8);//正常显示
        oled_write_cmd(0xA1);
    }
    if(arg == TURNOVER_DISPLAY)
    {
        oled_write_cmd(0xC0);//翻转显示
        oled_write_cmd(0xA0);
    }    
}

void ShowString(unsigned char x,unsigned char y, char *c,CHAR_SIZE size)
{
    while ((*c>=' ')&&(*c<='~')) //判断不是非法字符
    {
        ShowChar(x,y,*c,size);
        x+=size/2;
        if(x>(128-size))  //换行
        {
            x=0;
            y+=2;
        }
        c++;
    }  
}

void ShowChar(unsigned char x,unsigned char y,unsigned char c,CHAR_SIZE size)
{
    int oneCharNum = 0;
    unsigned char temp;
    int y0 = y;
    unsigned int offset = c - ' '; //计算偏移后的值
    oneCharNum = (size/8+((size%8)?1:0))*(size/2);
    for(int i=0;i<oneCharNum;i++)
    {
        if(size == size1206)
            temp = asc2_1206[offset][i];
        else if(size == size1608)
            temp = asc2_1608[offset][i];
        else if(size == size2412)
            temp = asc2_2412[offset][i];
        for(int m=0;m<8;m++)
        {
            if(temp&0x80)
                DrawPoint(x,y);
            else
                ClearPoint(x,y);
            temp<<=1;
            y++;
            if((y-y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}


void DrawLine(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2)
{
    unsigned i,k,k1,k2;
    if((x1<0)||(x2>128)||(y1<0)||(y2>64)||(x1>x2)||(y1>y2))
        return;
    if(x1==x2) //画竖线
    {
        for(i=0;i<(y2-y1);i++)
        {
            DrawPoint(x1,y1+i);
        }
    }
    else if(y1==y2)   //画横线
	{
		for(i=0;i<(x2-x1);i++)
		{
			DrawPoint(x1+i,y1);
		}
    }
    else      //画斜线
	{
		k1=y2-y1;
		k2=x2-x1;
		k=k1*10/k2;
		for(i=0;i<(x2-x1);i++)
		{
			DrawPoint(x1+i,y1+i*k/10);
		}
	}
}
void DrawCircle(unsigned char x,unsigned char y,unsigned char r)
{
    int a,b,num;
    a=0;
    b=r;
    while(2*b*b>=r*r)
    {
        DrawPoint(x+a,y-b);
        DrawPoint(x-a,y-b);
        DrawPoint(x-a,y+b);
        DrawPoint(x+a,y+b);

        DrawPoint(x+b,y+a);
        DrawPoint(x+b,y-a);
        DrawPoint(x-b,y-a);
        DrawPoint(x-b,y+a);

        a++;
        num = (a*a+b*b)-r*r;//计算画的点离圆心的距离
        if(num>0)
        {
            a--;
            b--;
        }
    }
}
void DrawPoint(unsigned char x,unsigned char y)
{
    ssd1306handler->oledBuffer[x][y/8] |= (1<<(y%8));
}
void ClearPoint(unsigned char x,unsigned char y)
{
    ssd1306handler->oledBuffer[x][y/8] =~ ssd1306handler->oledBuffer[x][y/8];
    ssd1306handler->oledBuffer[x][y/8] |= (1<<(y%8));
    ssd1306handler->oledBuffer[x][y/8] =~ ssd1306handler->oledBuffer[x][y/8];
}
void Refresh()
{
    //printf("reflash oled!\n");
    for(int i=0;i<8;i++)
    {
        oled_write_cmd(0xb0+i);
        oled_write_cmd(0x00);
        oled_write_cmd(0x10);
        for(int j=0;j<128;j++)
        {
           oled_write_data(ssd1306handler->oledBuffer[j][i]);
        }
    }
}

void oled_free()
{
    IIC_Free(ssd1306handler->iic);
    free(ssd1306handler);
}

