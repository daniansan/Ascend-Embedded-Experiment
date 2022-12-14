#include"v4L2_v2.h"
 
 
#define CAMERA_DEVICE "/dev/video0"
 
 
#define CAPTURE_FILE "frame_yuyv_new.jpg"
#define CAPTURE_RGB_FILE "frame_rgb_new.bmp"
#define CAPTURE_YUYV_CAP "frame_yuyv_cap.yuv"
 
 
#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define VIDEO_FORMAT V4L2_PIX_FMT_YUYV
#define BUFFER_COUNT 4
 
 
typedef struct VideoBuffer {
    void   *start; //视频缓冲区的起始地址
    size_t  length;//缓冲区的长度
} VideoBuffer;
 
 
/*
void *calloc(unsigned n,unsigned size) 功 能: 在内存的动态存储区中分配n个长度为size的连续空间，函数返回一个指向分配起始地址的指针；如果分配不成功，返回NULL。跟malloc的区别：calloc在动态分配完内存后，自动初始化该内存空间为零，而malloc不初始化，里边数据是随机的垃圾数据
*/
//位图文件头数据结构含有位图文件的类型，大小和打印格式等信息
//进行数据字节的对齐
#pragma pack(1)
typedef struct BITMAPFILEHEADER
{
  unsigned short bfType;//位图文件的类型,
  unsigned int bfSize;//位图文件的大小，以字节为单位
  unsigned short bfReserved1;//位图文件保留字，必须为0
  unsigned short bfReserved2;//同上
  unsigned int bfOffBits;//位图阵列的起始位置，以相对于位图文件   或者说是头的偏移量表示，以字节为单位
} BITMAPFILEHEADER;

 
 
typedef struct BITMAPINFOHEADER//位图信息头类型的数据结构，用于说明位图的尺寸
{
  unsigned int biSize;//位图信息头的长度，以字节为单位
  unsigned int biWidth;//位图的宽度，以像素为单位
  unsigned int biHeight;//位图的高度，以像素为单位
  unsigned short biPlanes;//目标设备的级别,必须为1
  unsigned short biBitCount;//每个像素所需的位数，必须是1(单色),4(16色),8(256色)或24(2^24色)之一
  unsigned int biCompression;//位图的压缩类型，必须是0-不压缩，1-BI_RLE8压缩类型或2-BI_RLE4压缩类型之一
  unsigned int biSizeImage;//位图大小，以字节为单位
  unsigned int biXPelsPerMeter;//位图目标设备水平分辨率，以每米像素数为单位
  unsigned int biYPelsPerMeter;//位图目标设备垂直分辨率，以每米像素数为单位
  unsigned int biClrUsed;//位图实际使用的颜色表中的颜色变址数
  unsigned int biClrImportant;//位图显示过程中被认为重要颜色的变址数
} BITMAPINFOHEADER;
#pragma pack()
 
 
 
VideoBuffer framebuf[BUFFER_COUNT];   
int fd;
struct v4l2_capability cap;
struct v4l2_fmtdesc fmtdesc;
struct v4l2_format fmt;
struct v4l2_requestbuffers reqbuf;
struct v4l2_buffer buf;
unsigned char *starter;
unsigned char *newBuf;
struct BITMAPFILEHEADER bfh;
struct BITMAPINFOHEADER bih;
 
 
void create_bmp_header()
{
  bfh.bfType = (unsigned short)0x4D42;
  bfh.bfSize = (unsigned long)(14 + 40 + VIDEO_WIDTH * VIDEO_HEIGHT*3);
  bfh.bfReserved1 = 0;
  bfh.bfReserved2 = 0;
  bfh.bfOffBits = (unsigned long)(14 + 40);
 
 
  bih.biBitCount = 24;
  bih.biWidth = VIDEO_WIDTH;
  bih.biHeight = VIDEO_HEIGHT;
  bih.biSizeImage = VIDEO_WIDTH * VIDEO_HEIGHT * 3;
  bih.biClrImportant = 0;
  bih.biClrUsed = 0;
  bih.biCompression = 0;
  bih.biPlanes = 1;
  bih.biSize = 40;//sizeof(bih);
  bih.biXPelsPerMeter = 0x00000ec4;
  bih.biYPelsPerMeter = 0x00000ec4;
}
 
 
int open_device()
{
/*
在linux下设备都是以文件的形式进行管理的
ioctl是设备驱动程序中对设备的I/O通道进行管理的函数int ioctl(int fd,int cmd,...)?
成功返回0，出错返回-1
其中fd--就是用户程序打开设备使用open函数返回的文件标识符
    cmd--就是用户程序对设备的控制命令，至于后面都省略号，有或没有和cmd的意义相关
*/
	int fd;
    fd = open(CAMERA_DEVICE, O_RDWR, 0);//
    if (fd < 0) {
        printf("Open %s failed\n", CAMERA_DEVICE);
        return -1;
    }
	return fd;
}
 
 
void get_capability()
{// 获取驱动信息
/*
控制命令VIDIOC_QUERYCAP
功能：查询设备驱动的功能;
参数说明：参数类型为V4L2的能力描述类型struct v4l2_capability;
struct v4l2_capability {
        __u8    driver[16];     //i.e. "bttv"            //驱动名称,
        __u8    card[32];       // i.e. "Hauppauge WinTV"         //
        __u8    bus_info[32];   // "PCI:" + pci_name(pci_dev)     //PCI总线信息
        __u32   version;        // should use KERNEL_VERSION() 
        __u32   capabilities;   // Device capabilities         //设备能力
        __u32   reserved[4];
};
返回值说明： 执行成功时，函数返回值为 0;
函数执行成功后，struct v4l2_capability 结构体变量中的返回当前视频设备所支持的功能
例如支持视频捕获功能V4L2_CAP_VIDEO_CAPTURE或V4L2_CAP_STREAMING
*/
    int ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
	if (ret < 0) {
        printf("VIDIOC_QUERYCAP failed (%d)\n", ret);
        return;
    }
    // Print capability infomations
    printf("------------VIDIOC_QUERYCAP-----------\n");
    printf("Capability Informations:\n");
    printf(" driver: %s\n", cap.driver);
    printf(" card: %s\n", cap.card);
    printf(" bus_info: %s\n", cap.bus_info);
    printf(" version: %08X\n", cap.version);
    printf(" capabilities: %08X\n\n", cap.capabilities);
	return;
}
 
 
void get_format()
{
/*获取当前视频设备支持的视频格式
控制命令 VIDIOC_ENUM_FMT
功能： 获取当前视频设备支持的视频格式 。
参数说明：参数类型为V4L2的视频格式描述符类型 struct v4l2_fmtdesc
struct v4l2_fmtdesc {
        __u32               index;             // Format number      
        enum v4l2_buf_type  type;              // buffer type        
        __u32               flags;
        __u8                description[32];   // Description string 
        __u32               pixelformat;       // Format fourcc      
        __u32               reserved[4];
};
返回值说明： 执行成功时，函数返回值为 0;
*/
	int ret;
	fmtdesc.index=0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret=ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc);
	while (ret != 0)
    {
        fmtdesc.index++;
		ret=ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc);
    }
	printf("--------VIDIOC_ENUM_FMT---------\n");
    printf("get the format what the device support\n{ pixelformat = ''%c%c%c%c'', description = ''%s'' }\n",fmtdesc.pixelformat & 0xFF, (fmtdesc.pixelformat >> 8) & 0xFF, (fmtdesc.pixelformat >> 16) & 0xFF,(fmtdesc.pixelformat >> 24) & 0xFF, fmtdesc.description);
    
	return;
}
 
 
int set_format()
{
/*
控制命令VIDIOC_S_FMT
功能：设置视频设备的视频数据格式，例如设置视频图像数据的长、宽，图像格式JPEG、YUYV格式);
参数说明：参数类型为V4L2的视频数据格式类型struct v4l2_format;
struct v4l2_format {
        enum v4l2_buf_type type;    //数据流类型，必须永远是V4L2_BUF_TYPE_VIDEO_CAPTURE
        union {
                struct v4l2_pix_format          pix;     // V4L2_BUF_TYPE_VIDEO_CAPTURE 
                struct v4l2_window              win;     // V4L2_BUF_TYPE_VIDEO_OVERLAY 
                struct v4l2_vbi_format          vbi;     // V4L2_BUF_TYPE_VBI_CAPTURE 
                struct v4l2_sliced_vbi_format   sliced;  // V4L2_BUF_TYPE_SLICED_VBI_CAPTURE 
                __u8    raw_data[200];                   // user-defined 
        } fmt;
};
struct v4l2_pix_format {
        __u32                   width;         // 宽，必须是16的倍数
        __u32                   height;        // 高，必须是16的倍数
        __u32                   pixelformat;   // 视频数据存储类型，例如是YUV4:2:2还是RGB
        enum v4l2_field       field;
        __u32                   bytesperline;
        __u32                   sizeimage;
        enum v4l2_colorspace colorspace;
        __u32                   priv;
};
返回值说明： 执行成功时，函数返回值为 0;
注意：如果该视频设备驱动不支持你所设定的图像格式，视频驱动会重新修改struct v4l2_format结构体变量的值为该视频设备所支持的图像格式，所以在程序设计中，设定完所有的视频格式后，要获取实际的视频格式，要重新读取 struct v4l2_format结构体变量。
使用VIDIOC_G_FMT设置视频设备的视频数据格式，VIDIOC_TRY_FMT验证视频设备的视频数据格式
*/
	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = VIDEO_WIDTH;
    fmt.fmt.pix.height      = VIDEO_HEIGHT;
    fmt.fmt.pix.pixelformat = fmtdesc.pixelformat;//V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
    int ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
        printf("VIDIOC_S_FMT failed (%d)\n", ret);
        return;
    }
 
 
  /*  // 设置视频格式VIDIOC_G_FMT和VIDIOC_S_FMT相同
    ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
    if (ret < 0) {
        printf("VIDIOC_G_FMT failed (%d)\n", ret);
        return ret;
    }*/
    // Print Stream Format
	printf("------------VIDIOC_S_FMT---------------\n");
    printf("Stream Format Informations:\n");
    printf(" type: %d\n", fmt.type);
    printf(" width: %d\n", fmt.fmt.pix.width);
    printf(" height: %d\n", fmt.fmt.pix.height);
 
 
    char fmtstr[8];
    memset(fmtstr, 0, 8);
/*
void *memcpy(void *dest, const void *src, size_t n);
从源src所指的内存地址的起始位置开始拷贝n个字节到目标dest所指的内存地址的起始位置中
所需头文件include <string.h>
*/
    memcpy(fmtstr, &fmt.fmt.pix.pixelformat, 4);
    printf(" pixelformat: %s\n", fmtstr);
    printf(" field: %d\n", fmt.fmt.pix.field);
    printf(" bytesperline: %d\n", fmt.fmt.pix.bytesperline);
    printf(" sizeimage: %d\n", fmt.fmt.pix.sizeimage);
    printf(" colorspace: %d\n", fmt.fmt.pix.colorspace);
    printf(" priv: %d\n", fmt.fmt.pix.priv);
    printf(" raw_date: %s\n", fmt.fmt.raw_data);
	return 0;
}
 
 
void request_buf()
{
/*
控制命令VIDIOC_REQBUFS
功能： 请求V4L2驱动分配视频缓冲区(申请V4L2视频驱动分配内存)，V4L2是视频设备的驱动层，位于内核空间，所以通过VIDIOC_REQBUFS控制命令字申请的内存位于内核空间，应用程序不能直接访问，需要通过调用mmap内存映射函数把内核空间内存映射到用户空间后，应用程序通过访问用户空间地址来访问内核空间。
参数说明：参数类型为V4L2的申请缓冲区数据结构体类型struct v4l2_requestbuffers;
struct v4l2_requestbuffers {
        u32                   count;        //缓存数量,也就是说在缓存队列里保持多少张照片
        enum v4l2_buf_type    type;         //数据流类型,必须永远是V4L2_BUF_TYPE_VIDEO_CAPTURE
        enum v4l2_memory      memory;       //V4L2_MEMORY_MMAP或V4L2_MEMORY_USERPTR
        u32                   reserved[2];
};
返回值说明： 执行成功时，函数返回值为 0，V4L2驱动层分配好了视频缓冲区;
注意：VIDIOC_REQBUFS会修改tV4L2_reqbuf的count值，tV4L2_reqbuf的count值返回实际申请成功的视频缓冲区数目;
*/
    reqbuf.count = BUFFER_COUNT;
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    
    int ret = ioctl(fd , VIDIOC_REQBUFS, &reqbuf);
    if(ret < 0) {
        printf("VIDIOC_REQBUFS failed (%d)\n", ret);
        return;
    }
	printf("the buffer has been assigned successfully!\n");
	return;
}

//void store_yuyv_cap(int i){
    
//     fwrite(framebuf[i].start, 1, buf.length, fp2);
//     fclose(fp2);
// }
 
void query_map_qbuf()
{
/*
控制命令VIDIOC_QUERYBUF
功能：查询已经分配的V4L2的视频缓冲区的相关信息，包括视频缓冲区的使用状态、在内核空间的偏移地址、缓冲区长度等。在应用程序设计中通过调VIDIOC_QUERYBUF来获取内核空间的视频缓冲区信息，然后调用函数mmap把内核空间地址映射到用户空间，这样应用程序才能够访问位于内核空间的视频缓冲区
参数说明：参数类型为V4L2缓冲区数据结构类型struct v4l2_buffer;
struct v4l2_buffer {
        __u32                   index;
        enum v4l2_buf_type      type;
        __u32                   bytesused;
        __u32                   flags;
        enum v4l2_field         field;
        struct timeval          timestamp;
        struct v4l2_timecode    timecode;
        __u32                   sequence;
        // memory location 
        enum v4l2_memory        memory;
        union {
                __u32           offset;
                unsigned long   userptr;
        } m;
        __u32                   length;
        __u32                   input;
        __u32                   reserved;
};
返回值说明： 执行成功时，函数返回值为 0;
struct v4l2_buffer结构体变量中保存了指令的缓冲区的相关信息;一般情况下，应用程序中调用VIDIOC_QUERYBUF取得了内核缓冲区信息后，紧接着调用mmap函数把内核空间地址映射到用户空间方便用户空间应用程序的访问
*/
	int i,ret;
    for (i = 0; i < reqbuf.count; i++)
    {
        buf.index = i;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        ret = ioctl(fd , VIDIOC_QUERYBUF, &buf);//buf取得内存缓冲区的信息
        if(ret < 0) {
            printf("VIDIOC_QUERYBUF (%d) failed (%d)\n", i, ret);
            return;
        }
 
 
        // mmap buffer
        framebuf[i].length = buf.length;//framebuf是程序最前面定义的一个结构体类型的数据
/*
#include <sys/mman.h>
void *mmap(void *start, size_t length, int prot, int flags,int fd, off_t offset);
int munmap(void *start, size_t length);
mmap将一个文件或者其它对象映射进内存。文件被映射到多个页上，如果文件的大小不是所有页的大小之和，最后一个页不被使用的空间将会清零
start：映射区的开始地址，设置为0时表示由系统决定映射区的起始地址
length：映射区的长度
prot：期望的内存保护标志，不能与文件的打开模式冲突。是以下的某个值，可以通过or运算合理地组合在一起	PROT_EXEC //页内容可以被执行
	PROT_READ //页内容可以被读取
	PROT_WRITE //页可以被写入
	PROT_NONE //页不可访问
flags：指定映射对象的类型，映射选项和映射页是否可以共享
	MAP_SHARED //与其它所有映射这个对象的进程共享映射空间。对共享区的写入，相当于输出到文件。直到msync()或者munmap()被调用，文件实际上不会被更新
fd：有效的文件描述词。一般是由open()函数返回，其值也可以设置为-1，此时需要指定flags参数中的MAP_ANON,表明进行的是匿名映射
offset：被映射对象内容的起点
成功执行时，mmap()返回被映射区的指针，munmap()返回0。失败时，mmap()返回MAP_FAILED[其值为(void *)-1]，munmap返回-1
*/
        framebuf[i].start = (char *) mmap(0, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (framebuf[i].start == MAP_FAILED) {
            printf("mmap (%d) failed: %s\n", i, strerror(errno));
            return;
        }
        // Queen buffer
/*
控制命令VIDIOC_QBUF
功能：投放一个空的视频缓冲区到视频缓冲区输入队列中
参数说明：参数类型为V4L2缓冲区数据结构类型struct v4l2_buffer;
返回值说明： 执行成功时，函数返回值为 0;
函数执行成功后，指令(指定)的视频缓冲区进入视频输入队列，在启动视频设备拍摄图像时，相应的视频数据被保存到视频输入队列相应的视频缓冲区中
*/
        ret = ioctl(fd , VIDIOC_QBUF, &buf);
        if (ret < 0) {
            printf("VIDIOC_QBUF (%d) failed (%d)\n", i, ret);
            return;
        }        
        printf("Frame buffer %d: address=0x%x, length=%d\n", i, (unsigned int)framebuf[i].start, framebuf[i].length);
    }//空的视频缓冲区都已经在视频缓冲的输入队列中了
	return;
}
 
 
 
 
void yuyv2rgb()
{
    unsigned char YUYV[4],RGB[6];
    int j,k,i;   
	unsigned int location=0;
	j=0;
    for(i=0;i < framebuf[buf.index].length;i+=4)
    {
		YUYV[0]=starter[i];//Y0
		YUYV[1]=starter[i+1];//U
		YUYV[2]=starter[i+2];//Y1
		YUYV[3]=starter[i+3];//V
		if(YUYV[0]<1)
		{
			RGB[0]=0;
			RGB[1]=0;
			RGB[2]=0;
		}
		else
		{
			RGB[0]=YUYV[0]+1.772*(YUYV[1]-128);//b
			RGB[1]=YUYV[0]-0.34413*(YUYV[1]-128)-0.71414*(YUYV[3]-128);//g
			RGB[2]=YUYV[0]+1.402*(YUYV[3]-128);//r
		}
		if(YUYV[2]<0)
		{
			RGB[3]=0;
			RGB[4]=0;
			RGB[5]=0;
		}
		else
		{
			RGB[3]=YUYV[2]+1.772*(YUYV[1]-128);//b
			RGB[4]=YUYV[2]-0.34413*(YUYV[1]-128)-0.71414*(YUYV[3]-128);//g
			RGB[5]=YUYV[2]+1.402*(YUYV[3]-128);//r
 
 
		}
 
 
		for(k=0;k<6;k++)
		{
			if(RGB[k]<0)
				RGB[k]=0;
			if(RGB[k]>255)
				RGB[k]=255;
		}
 
 
		//请记住：扫描行在位图文件中是反向存储的！
		if(j%(VIDEO_WIDTH*3)==0)//定位存储位置
		{
			location=(VIDEO_HEIGHT-j/(VIDEO_WIDTH*3))*(VIDEO_WIDTH*3);
		}
		bcopy(RGB,newBuf+location+(j%(VIDEO_WIDTH*3)),sizeof(RGB));
 
 
		j+=6;		
    }
	return;
}
 
 
void move_noise()
{//双滤波器
	int i,j,k,temp[3],temp1[3];
	unsigned char BGR[13*3];
	unsigned int sq,sq1,loc,loc1;
	int h=VIDEO_HEIGHT,w=VIDEO_WIDTH;
	for(i=2;i<h-2;i++)
	{
		for(j=2;j<w-2;j++)
		{
			memcpy(BGR,newBuf+(i-1)*w*3+3*(j-1),9);
			memcpy(BGR+9,newBuf+i*w*3+3*(j-1),9);
			memcpy(BGR+18,newBuf+(i+1)*w*3+3*(j-1),9);
			memcpy(BGR+27,newBuf+(i-2)*w*3+3*j,3);
			memcpy(BGR+30,newBuf+(i+2)*w*3+3*j,3);
			memcpy(BGR+33,newBuf+i*w*3+3*(j-2),3);
			memcpy(BGR+36,newBuf+i*w*3+3*(j+2),3);
 
 
			memset(temp,0,4*3);
			
			for(k=0;k<9;k++)
			{
				temp[0]+=BGR[k*3];
				temp[1]+=BGR[k*3+1];
				temp[2]+=BGR[k*3+2];
			}
			temp1[0]=temp[0];
			temp1[1]=temp[1];
			temp1[2]=temp[2];
			for(k=9;k<13;k++)
			{
				temp1[0]+=BGR[k*3];
				temp1[1]+=BGR[k*3+1];
				temp1[2]+=BGR[k*3+2];
			}
			for(k=0;k<3;k++)
			{
				temp[k]/=9;
				temp1[k]/=13;
			}
			sq=0xffffffff;loc=0;
			sq1=0xffffffff;loc1=0;
			unsigned int a;			
			for(k=0;k<9;k++)
			{
				a=abs(temp[0]-BGR[k*3])+abs(temp[1]-BGR[k*3+1])+abs(temp[2]-BGR[k*3+2]);
				if(a<sq)
				{
					sq=a;
					loc=k;
				}
			}
			for(k=0;k<13;k++)
			{
				a=abs(temp1[0]-BGR[k*3])+abs(temp1[1]-BGR[k*3+1])+abs(temp1[2]-BGR[k*3+2]);
				if(a<sq1)
				{
					sq1=a;
					loc1=k;
				}
			}
			
			newBuf[i*w*3+3*j]=(unsigned char)((BGR[3*loc]+BGR[3*loc1])/2);
			newBuf[i*w*3+3*j+1]=(unsigned char)((BGR[3*loc+1]+BGR[3*loc1+1])/2);
			newBuf[i*w*3+3*j+2]=(unsigned char)((BGR[3*loc+2]+BGR[3*loc1+2])/2);
			/*还是有些许的噪点
			temp[0]=(BGR[3*loc]+BGR[3*loc1])/2;
			temp[1]=(BGR[3*loc+1]+BGR[3*loc1+1])/2;
			temp[2]=(BGR[3*loc+2]+BGR[3*loc1+2])/2;
			sq=abs(temp[0]-BGR[loc*3])+abs(temp[1]-BGR[loc*3+1])+abs(temp[2]-BGR[loc*3+2]);
			sq1=abs(temp[0]-BGR[loc1*3])+abs(temp[1]-BGR[loc1*3+1])+abs(temp[2]-BGR[loc1*3+2]);
			if(sq1<sq) loc=loc1;
			newBuf[i*w*3+3*j]=BGR[3*loc];
			newBuf[i*w*3+3*j+1]=BGR[3*loc+1];
			newBuf[i*w*3+3*j+2]=BGR[3*loc+2];*/
		}
	}
	return;
}
 
 
void yuyv2rgb1()
{
    unsigned char YUYV[3],RGB[3];
	memset(YUYV,0,3);
    int j,k,i;   
	unsigned int location=0;
	j=0;
    for(i=0;i < framebuf[buf.index].length;i+=2)
    {
		YUYV[0]=starter[i];//Y0
		if(i%4==0)
			YUYV[1]=starter[i+1];//U
		//YUYV[2]=starter[i+2];//Y1
		if(i%4==2)
			YUYV[2]=starter[i+1];//V
		if(YUYV[0]<1)
		{
			RGB[0]=0;
			RGB[1]=0;
			RGB[2]=0;
		}
		else
		{
			RGB[0]=YUYV[0]+1.772*(YUYV[1]-128);//b
			RGB[1]=YUYV[0]-0.34413*(YUYV[1]-128)-0.71414*(YUYV[2]-128);//g
			RGB[2]=YUYV[0]+1.402*(YUYV[2]-128);//r
		}
 
 
		for(k=0;k<3;k++)
		{
			if(RGB[k]<0)
				RGB[k]=0;
			if(RGB[k]>255)
				RGB[k]=255;
		}
 
 
		//请记住：扫描行在位图文件中是反向存储的！
		if(j%(VIDEO_WIDTH*3)==0)//定位存储位置
		{
			location=(VIDEO_HEIGHT-j/(VIDEO_WIDTH*3))*(VIDEO_WIDTH*3);
		}
		bcopy(RGB,newBuf+location+(j%(VIDEO_WIDTH*3)),sizeof(RGB));
 
 
		j+=3;		
    }
	return;
}
 
 
void store_yuyv()
{
    FILE *fp = fopen(CAPTURE_FILE, "wb");
    if (fp < 0) {
        printf("open frame data file failed\n");
        return;
    }
    fwrite(framebuf[buf.index].start, 1, buf.length, fp);//写死为第一张
    fclose(fp);
    printf("Capture one frame saved in %s\n", CAPTURE_FILE);
    return;
}
 
 
 
 
void store_bmp(int n_len)
{
	FILE *fp1 = fopen(CAPTURE_RGB_FILE, "wb");
    if (fp1 < 0) {
        printf("open frame data file failed\n");
        return;
    }
    printf("Bfh: %d,Bin: %d\n", sizeof(BITMAPFILEHEADER),sizeof(BITMAPINFOHEADER));
	fwrite(&bfh,sizeof(BITMAPFILEHEADER),1,fp1);
	fwrite(&bih,sizeof(BITMAPINFOHEADER),1,fp1);
	fwrite(newBuf, 1, n_len, fp1);
    fclose(fp1);
    printf("Change one frame saved in %s\n", CAPTURE_RGB_FILE);
	return;
}
 
 
v4l2_test()
{
    int i, ret;
    FILE *fp2 = fopen(CAPTURE_YUYV_CAP, "wb");
    if (fp2 < 0) {
        printf("open frame data file failed\n");
        return -1;
    }

    // 打开设备
    fd=open_device();
    
    // 获取驱动信息
    //struct v4l2_capability cap;
    get_capability();
 	
	//获取当前视频设备支持的视频格式
    //struct v4l2_fmtdesc fmtdesc;
    memset(&fmtdesc,0,sizeof(fmtdesc));
	get_format();
	
    // 设置视频格式
    //struct v4l2_format fmt;
    //memset在一段内存块中填充某个给定的值，它是对较大的结构体或数组进行清零操作的一种最快的方法
    memset(&fmt, 0, sizeof(fmt));//将fmt中的前sizeof(fmt)字节用0替换并返回fmt
    set_format();
    
    // 请求分配内存
	//struct v4l2_requestbuffers reqbuf;
	request_buf();
 
 
    // 获取空间，并将其映射到用户空间，然后投放到视频输入队列
	//struct v4l2_buffer buf;
	query_map_qbuf();
 
 
 
 
    // 开始录制
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
/*
控制命令VIDIOC_STREAMON
功能：启动视频采集命令，应用程序调用VIDIOC_STREAMON启动视频采集命令后，视频设备驱动程序开始采集视频数据，并把采集到的视频数据保存到视频驱动的视频缓冲区中
参数说明：参数类型为V4L2的视频缓冲区类型 enum v4l2_buf_type ;
enum v4l2_buf_type {
        V4L2_BUF_TYPE_VIDEO_CAPTURE        = 1,
        V4L2_BUF_TYPE_VIDEO_OUTPUT         = 2,
        V4L2_BUF_TYPE_VIDEO_OVERLAY        = 3,
        V4L2_BUF_TYPE_VBI_CAPTURE          = 4,
        V4L2_BUF_TYPE_VBI_OUTPUT           = 5,
        V4L2_BUF_TYPE_SLICED_VBI_CAPTURE   = 6,
        V4L2_BUF_TYPE_SLICED_VBI_OUTPUT    = 7,
#if 1
         Experimental 
        V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY = 8,
#endif
        V4L2_BUF_TYPE_PRIVATE              = 0x80,
};
返回值说明： 执行成功时，函数返回值为 0;函数执行成功后，视频设备驱动程序开始采集视频数据，此时应用程序一般通过调用select函数来判断一帧视频数据是否采集完成，当视频设备驱动完成一帧视频数据采集并保存到视频缓冲区中时，select函数返回，应用程序接着可以读取视频数据;否则select函数阻塞直到视频数据采集完成
*/
    ret = ioctl(fd, VIDIOC_STREAMON, &type);
    if (ret < 0) {
        printf("VIDIOC_STREAMON failed (%d)\n", ret);
        return ret;
    }
    for (int i = 0; i < BUFFER_COUNT; i++)
    {   
        ret = ioctl(fd, VIDIOC_DQBUF, &buf);
        if (ret < 0) {
        printf("VIDIOC_DQBUF failed (%d)\n", ret);
        return ret;
        }
        fwrite(framebuf[i].start, 1, buf.length, fp2);
        printf("55555555:%d\n",i);

        ret = ioctl(fd, VIDIOC_QBUF, &buf);
         if (ret < 0) {
        printf("VIDIOC_QBUF failed (%d)\n", ret);
        return ret;
        }
    }
    
    
 
    // Get frame

    ret = ioctl(fd, VIDIOC_DQBUF, &buf);//VIDIOC_DQBUF命令结果, 使从队列删除的缓冲帧信息传给了此buf
    if (ret < 0) {
        printf("VIDIOC_DQBUF failed (%d)\n", ret);
        return ret;
    }
    // Process the frame 此时我们需要进行数据格式的改变
	store_yuyv();
    
    /*结束视频采集*/
	if (0 > ioctl(fd, VIDIOC_STREAMOFF, &type)) 
	{
		perror("ioctl error");
		return -1;
	}
    
    //对采集的数据进行转变，变换成RGB24模式，然后进行存储
/*
（1）开辟出来一段内存区域来存放转换后的数据
（2）循环读取buf内存段的内容，进行转换，转换后放入到新开辟的内存区域中
（3）将新开辟出来的内存区的内容读到文件中
*/
    printf("********************************************\n");
    int n_len;
    n_len=framebuf[buf.index].length*3/2;
    newBuf=calloc((unsigned int)n_len,sizeof(unsigned char));
  
    if(!newBuf)
    {
		printf("cannot assign the memory !\n");
		exit(0);
    }
 
 
    printf("the information about the new buffer:\n start Address:0x%x,length=%d\n\n",(unsigned int)newBuf,n_len);
 
 
	printf("----------------------------------\n");
	
	//YUYV to RGB
    starter=(unsigned char *)framebuf[buf.index].start;
	yuyv2rgb();//还是这个采集的图片的效果比较好
	move_noise();
	//yuyv2rgb1();
	//设置bmp文件的头和bmp文件的一些信息
	create_bmp_header();
	
	store_bmp(n_len);
	
	
    // Re-queen buffer
    ret = ioctl(fd, VIDIOC_QBUF, &buf);
    if (ret < 0) {
        printf("VIDIOC_QBUF failed (%d)\n", ret);
        return ret;
    }
	printf("re-queen buffer end\n");
    // Release the resource
/*
表头文件 #include<unistd.h>
        #include<sys/mman.h>
        定义函数 int munmap(void *start,size_t length);
        函数说明 munmap()用来取消参数start所指的映射内存起始地址，参数length则是欲取消的内存大小。当进程结束或利用exec相关函数来执行其他程序时，映射内存会自动解除，但关闭对应的文件描述词时不会解除映射
        返回值 如果解除映射成功则返回0，否则返回－1
*/
    for (i=0; i< 4; i++)
    {
	
        munmap(framebuf[i].start, framebuf[i].length);
    }
	//free(starter);
printf("free starter end\n");
	//free(newBuf);
printf("free newBuf end\n");
    close(fd);
 
 
	
    printf("Camera test Done.\n");
    return 0;
}
