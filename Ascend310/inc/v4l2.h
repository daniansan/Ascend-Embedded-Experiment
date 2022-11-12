#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#include <linux/videodev2.h>
#include <linux/fb.h>

#define BUFFER_COUNT 5
#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define RGB_SIZE 921600

void v4l2_test();
