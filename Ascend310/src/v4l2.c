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

//void yuyv_to_rgb(unsigned char* yuv,unsigned char* rgb);
//void rgb_to_bmp(unsigned char* pdata, FILE* bmp_fd);
//void rgb_stretch(char* src_buf, char* dest_buf, int des_width, int des_hight);

int v4l2_test()
{
    int fd;
	int res;
    FILE *file_fd = fopen("v4l2_cap.yuv","w+");
    FILE *bmp_fd = fopen("v4l2_cap.bmp","w+");
    char rgb_buffers[RGB_SIZE];
    char dest_buffers[RGB_SIZE];
	fd = open("/dev/video0",O_RDWR);
	if(fd<0)
	{
		printf("open error %s：%s\n","/dev/video0",strerror(errno));
		return 0;
	}

    /*获取设置当前摄像头配置*/
	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(fd,VIDIOC_G_FMT,&fmt)<0)
		return 0;
	printf("Get width:%d, height:%d format:0x%x\n",
		fmt.fmt.pix.width, fmt.fmt.pix.height, fmt.fmt.pix.pixelformat);
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.width = VIDEO_WIDTH;
	fmt.fmt.pix.height = VIDEO_HEIGHT;
	if(ioctl(fd,VIDIOC_S_FMT,&fmt)<0)
		return 0;
	printf("Set width:%d, height:%d format:0x%x\n",
		fmt.fmt.pix.width, fmt.fmt.pix.height, fmt.fmt.pix.pixelformat);
    
    
   	/* 申请帧缓冲、内存映射*/
	struct v4l2_requestbuffers reqbuf;
	struct v4l2_buffer buf;
	
	void *frm_base[BUFFER_COUNT];
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuf.count = BUFFER_COUNT;
	reqbuf.memory = V4L2_MEMORY_MMAP;

	if (0 > ioctl(fd, VIDIOC_REQBUFS, &reqbuf)) 
	{
		printf("ioctl error: VIDIOC_REQBUFS: %s\n", strerror(errno));
		return -1;
	}
	/* 建立内存映射 */
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	for(buf.index = 0; buf.index < BUFFER_COUNT; buf.index++) 
	{
		ioctl(fd, VIDIOC_QUERYBUF, &buf);
		frm_base[buf.index] = mmap(NULL, buf.length,PROT_READ | PROT_WRITE, MAP_SHARED,fd, buf.m.offset);
    
		if (MAP_FAILED == frm_base[buf.index])
		{
			perror("mmap error");
			return -1;
		}
        /* 入队*/
  		if (0 > ioctl(fd, VIDIOC_QBUF, &buf))
		{
			perror("ioctl error");
			return -1;
		}    
	}
 
	/*开启视频采集*/
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (0 > ioctl(fd, VIDIOC_STREAMON, &type))
	{
		perror("ioctl error");
		return -1;
	}
 
 
	
	/*读取数据、队数据进行处理 */
    if(1)
	{
		for(buf.index = 0; buf.index < BUFFER_COUNT; buf.index++) 
		{
			ioctl(fd, VIDIOC_DQBUF, &buf); //出队
			// 读取帧缓冲的映射区、获取一帧数据
			// 处理这一帧数据

            //yuyv_to_rgb(frm_base[buf.index],rgb_buffers);
            //rgb_stretch(rgb_buffers, dest_buffers, 128,64);
            //rgb_to_bmp(dest_buffers, bmp_fd);
            
            fwrite(frm_base[buf.index],buf.bytesused,1,file_fd);
            
			// 数据处理完之后、将当前帧缓冲入队、接着读取下一帧数据
			ioctl(fd, VIDIOC_QBUF, &buf);
		}
	}
 
	/*结束视频采集*/
	if (0 > ioctl(fd, VIDIOC_STREAMOFF, &type)) 
	{
		perror("ioctl error");
		return -1;
	}
}
/*
void yuyv_to_rgb(unsigned char* yuv,unsigned char* rgb)
{
    unsigned int i;
    unsigned char* y0 = yuv + 0;   
    unsigned char* u0 = yuv + 1;
    unsigned char* y1 = yuv + 2;
    unsigned char* v0 = yuv + 3;
 
    unsigned  char* r0 = rgb + 0;
    unsigned  char* g0 = rgb + 1;
    unsigned  char* b0 = rgb + 2;
    unsigned  char* r1 = rgb + 3;
    unsigned  char* g1 = rgb + 4;
    unsigned  char* b1 = rgb + 5;
   
    float rt0 = 0, gt0 = 0, bt0 = 0, rt1 = 0, gt1 = 0, bt1 = 0;
 
    for(i = 0; i <= (WIDTH * HEIGHT) / 2 ;i++)
    {
        bt0 = 1.164 * (*y0 - 16) + 2.018 * (*u0 - 128); 
        gt0 = 1.164 * (*y0 - 16) - 0.813 * (*v0 - 128) - 0.394 * (*u0 - 128); 
        rt0 = 1.164 * (*y0 - 16) + 1.596 * (*v0 - 128); 
   
        bt1 = 1.164 * (*y1 - 16) + 2.018 * (*u0 - 128); 
        gt1 = 1.164 * (*y1 - 16) - 0.813 * (*v0 - 128) - 0.394 * (*u0 - 128); 
        rt1 = 1.164 * (*y1 - 16) + 1.596 * (*v0 - 128); 
    
      
        if(rt0 > 250)      rt0 = 255;
        if(rt0< 0)        rt0 = 0;    
 
        if(gt0 > 250)     gt0 = 255;
        if(gt0 < 0)    gt0 = 0;    
 
        if(bt0 > 250)    bt0 = 255;
        if(bt0 < 0)    bt0 = 0;    
 
        if(rt1 > 250)    rt1 = 255;
        if(rt1 < 0)    rt1 = 0;    
 
        if(gt1 > 250)    gt1 = 255;
        if(gt1 < 0)    gt1 = 0;    
 
        if(bt1 > 250)    bt1 = 255;
        if(bt1 < 0)    bt1 = 0;    
                    
        *r0 = (unsigned char)rt0;
        *g0 = (unsigned char)gt0;
        *b0 = (unsigned char)bt0;
    
        *r1 = (unsigned char)rt1;
        *g1 = (unsigned char)gt1;
        *b1 = (unsigned char)bt1;
 
        yuv = yuv + 4;
        rgb = rgb + 6;
        if(yuv == NULL)
          break;
 
        y0 = yuv;
        u0 = yuv + 1;
        y1 = yuv + 2;
        v0 = yuv + 3;
  
        r0 = rgb + 0;
        g0 = rgb + 1;
        b0 = rgb + 2;
        r1 = rgb + 3;
        g1 = rgb + 4;
        b1 = rgb + 5;
    }   
}

void rgb_to_bmp(unsigned char* pdata, FILE* bmp_fd)     
{
    //分别为rgb数据，要保存的bmp文件名 
    int size = WIDTH * HEIGHT * 3 * sizeof(char); // 每个像素点3个字节  
    // 位图第一部分，文件信息  
    BMPFILEHEADER_T bfh;  
    bfh.bfType = (unsigned short)0x4d42;  //bm  
    bfh.bfSize = size  // data size  
        + sizeof( BMPFILEHEADER_T ) // first section size  
        + sizeof( BMPINFOHEADER_T ) // second section size  
        ;  
    bfh.bfReserved1 = 0; // reserved  
    bfh.bfReserved2 = 0; // reserved  
    bfh.bfOffBits = sizeof( BMPFILEHEADER_T )+ sizeof( BMPINFOHEADER_T );//真正的数据的位置 
//    printf("bmp_head== %ld\n", bfh.bfOffBits); 
    // 位图第二部分，数据信息  
    BMPINFOHEADER_T bih;  
    bih.biSize = sizeof(BMPINFOHEADER_T);  
    bih.biWidth = WIDTH;  
    bih.biHeight = -HEIGHT;//BMP图片从最后一个点开始扫描，显示时图片是倒着的，所以用-height，这样图片就正了  
    bih.biPlanes = 1;//为1，不用改  
    bih.biBitCount = 24;  
    bih.biCompression = 0;//不压缩  
    bih.biSizeImage = size;  
 
    bih.biXPelsPerMeter = 0;//像素每米  
  
    bih.biYPelsPerMeter = 0;  
    bih.biClrUsed = 0;//已用过的颜色，为0,与bitcount相同  
    bih.biClrImportant = 0;//每个像素都重要   
    
    fwrite( &bfh, 8, 1, bmp_fd); 
    fwrite(&bfh.bfReserved2, sizeof(bfh.bfReserved2), 1, bmp_fd);  
    fwrite(&bfh.bfOffBits, sizeof(bfh.bfOffBits), 1, bmp_fd);  
    fwrite(&bih, sizeof(BMPINFOHEADER_T), 1, bmp_fd);  
    fwrite(pdata, size, 1, bmp_fd);   
} 

void rgb_stretch(char* src_buf, char* dest_buf, int des_width, int des_hight)
{
    //最临近插值算法
    //双线性内插值算法放大后马赛克很严重 而且帧率下降严重
    printf("des_width = %d, des_hight = %d \n ",des_width, des_hight);
    double rate_w = (double) WIDTH / des_width;//横向放大比
    double rate_h = (double) HEIGHT / des_hight;//轴向放大比
    
    int dest_line_size = ((des_width * BITCOUNT +31) / 32) * 4;    
    int src_line_size = BITCOUNT * WIDTH / 8;
    int i = 0, j = 0, k = 0;
    for (i = 0; i < des_hight; i++)//desH 目标高度
    {
        //选取最邻近的点
        int t_src_h = (int)(rate_h * i + 0.5);//rateH (double)srcH / desH;
        for (j = 0; j < des_width; j++)//desW 目标宽度
        {
            int t_src_w = (int)(rate_w * j + 0.5);                            
            memcpy(&dest_buf[i * dest_line_size] + j * BITCOUNT / 8, \
                &src_buf[t_src_h * src_line_size] + t_src_w * BITCOUNT / 8,\
                BITCOUNT / 8);            
        }
    }   
}
*/
