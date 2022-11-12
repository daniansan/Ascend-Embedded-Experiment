#include "uart.h"

int uart_open(int* fd,char* path)
{
    *fd = open(path,O_RDWR|O_NOCTTY|O_NDELAY);

    if (-1 == *fd)
    {
        printf("Can't Open ttyAMA1");
        return(-1);
    }
    else
    {
        printf("open ttyAMA1 success\n");
    }


     //recover uart block status to recevier datainput
    if(fcntl(*fd, F_SETFL, 0) < 0)
        printf("fcntl failed!\n");
    else
        printf("fcntl=%d\n",fcntl(*fd, F_SETFL,0));

    //check uart valid
    if(isatty(STDIN_FILENO)==0)
        printf("standard input is not a terminal device\n");
    else
        printf("isatty success!\n");

    printf("fd-open=%d\n",*fd);

    return 0;
}


int uart_close(int* fd)
{
    if(-1 == close(*fd))
    {
        printf("uart close fd is wrong! ");
        return -1;
    }
    else
    {
        printf("close ttyAMA1 success\n");
        return 0;
    }
}


int uart_send(int fd, char *buffer)
{
    size_t len  = strlen(buffer);
    return write(fd,buffer,len);
}


int uart_read(int fd, char *buffer)
{
    return read(fd,buffer,1024);
}


int uart_set_option(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;

    //backup current uart setting
    if ( tcgetattr( fd,&oldtio) != 0)
    {
        printf("SetupSerial 1");
        return -1;
    }

    bzero( &newtio, sizeof( newtio ) );

    //set char size
    newtio.c_cflag |= CLOCAL | CREAD;   //忽略调制解调器控制线 接收使能
    
    newtio.c_cflag &= ~CSIZE;
    //set data bit
    switch( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    //set verify bit
    switch( nEvent )
    {
    case 'O':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':
        newtio.c_cflag &= ~PARENB;
        break;
    }

    //set speed
    switch( nSpeed )
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 19200:
        cfsetispeed(&newtio, B19200);
        cfsetospeed(&newtio, B19200);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    default:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    }

    //set stop bit
    if( nStop == 1 )
        newtio.c_cflag &= ~CSTOPB;   //将停止位设置为1个比特
    else if ( nStop == 2 )
        newtio.c_cflag |= CSTOPB;    //将停止位设置为2个比特

    //set wait time and min receiver size
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;

    //clear receiver buffer
    tcflush(fd,TCIFLUSH);

    //reset setting
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
        printf("com set error");
        return -1;
    }
    printf("set uart new setting done!\n");
    return 0;    
}


void uart_clearbuffer(int fd)
{
    tcflush(fd,TCIFLUSH);
}



