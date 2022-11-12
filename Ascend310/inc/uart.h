#ifndef __UART_H_
#define __UART_H_
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

int uart_open(int* fd,char* pwd);
int uart_close(int* fd);
int uart_send(int fd, char *buffer);
int uart_read(int fd, char *buffer);
int uart_set_option(int fd, int nSpeed, int nBits, char nEvent, int nStop);
void uart_clearbuffer(int fd);

#endif //__UART_H_