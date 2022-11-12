
# 浙江工业大学计算机学院嵌入式实验课程代码仓

如使用自己电脑的同学，可按如下步骤完成实验环境搭建

1. 下载[软件工具 提取码：ief7](https://pan.baidu.com/s/1g2mzsRLWQQqHkE7AtDrvxQ)链接中的软件并完成安装，虚拟机必装，远程登录和文件传输工具可选择一个进行安装。
    
    Vmware 虚拟机(必装) [参考安装教程](https://blog.csdn.net/zp17834994071/article/details/107137626?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522166824266916800180662769%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=166824266916800180662769&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-107137626-null-null.142^v63^control,201^v3^control_2,213^v2^t3_esquery_v3&utm_term=VMware%E5%AE%89%E8%A3%85%E6%95%99%E7%A8%8B&spm=1018.2226.3001.4187)
    
    mobaXterm 远程登录和文件传输工具（选装）[参考安装和使用教程]([https://blog.csdn.net/m0_45463480/article/details/124672407?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522166824275516782391831099%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=166824275516782391831099&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-124672407-null-null.142^v63^control,201^v3^control_2,213^v2^t3_esquery_v3&utm_term=mobaXterm&spm=1018.2226.3001.4187](https://blog.csdn.net/xishining/article/details/122974472?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522166824303716800182742042%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=166824303716800182742042&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~baidu_landing_v2~default-5-122974472-null-null.142^v63^control,201^v3^control_2,213^v2^t3_esquery_v3&utm_term=MobaXterm&spm=1018.2226.3001.4187)
    
    putty 远程登录工具（选装）[参考安装和使用教程](https://blog.csdn.net/l707941510/article/details/80520790?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522166824280716782388016747%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=166824280716782388016747&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-80520790-null-null.142^v63^control,201^v3^control_2,213^v2^t3_esquery_v3&utm_term=putty&spm=1018.2226.3001.4187)
    
    FileZilla 文件传输工具（选装）[参考安装和使用教程](https://blog.csdn.net/weixin_45309916/article/details/107782070?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522166824285616782414942783%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=166824285616782414942783&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-107782070-null-null.142^v63^control,201^v3^control_2,213^v2^t3_esquery_v3&utm_term=FileZilla&spm=1018.2226.3001.4187)
    
    Xftp 文件传输工具（选装）[参考安装和使用教程](https://blog.csdn.net/weixin_44953395/article/details/112900516?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522166824287716782425669866%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=166824287716782425669866&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-112900516-null-null.142^v63^control,201^v3^control_2,213^v2^t3_esquery_v3&utm_term=Xftp&spm=1018.2226.3001.4187)
    
3. 下载[ubuntu镜像文件 提取码：1ihg](https://pan.baidu.com/s/12DJSe3D3q5WOnWw-PapQHA),使用Vmware创建ubuntu系统。
4. 下载[制卡脚本包 提取码：t93u](https://pan.baidu.com/s/1tcbG-bhtHZ82TKMsFNjkJQ)上传到ubuntu虚拟机系统，进行制卡。
5. git clone 或者下载本仓库的代码并上传到开发板环境下。
6. UART实验中使用ESP32C3前，需要对其进行固件烧写，固件烧写步骤和软件包在此[链接](https://pan.baidu.com/s/1MCGT-7nqMSKgsvG2ish1ww)下。提取码：ktko
7. 实验中的涉及模型文件转换步骤，请下载下面两个链接并完成文件夹创建和拷贝，然后进行模型转换。

    [在Ascend路径下创建model目录，并下载复制链接中的文件](https://pan.baidu.com/s/1JEF4IRlsSBlO6iS0Nm4vow) 提取码：uaja

    [在human_segmentation路径下创建model目录，并下载复制链接中的文件](https://pan.baidu.com/s/1VIr4VUSY-rVbWkKTu9pncw)提取码：0r5h


---
此[链接](https://pan.baidu.com/s/106wKvTo7EkgAhgziSybJSg)是整个实验工程及软件工具的下载链接（实验代码不一定是最新的）提取码：byqx
