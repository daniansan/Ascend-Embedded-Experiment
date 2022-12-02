cmake_minimum_required(VERSION 3.5.1)

project(Ascend310)

set(LIBRARY_OUTPUT_PATH  "../lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "../out")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fexec-charset=GBK")
set(CMAKE_CXX_FLAGS_DEBUG "-fPIC -O0 -g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-fPIC -O2 -Wall")
set(SRC_LIST 
    cJSON.c    
    esp32_c3.c  
    key.c       
    pca9557.c  
    spi.c      
    task.c
    adxl345.c       
    ds1339u.c  
    i2c.c       
    led.c  
    multiButton.c  
    sht20.c    
    ssd1306.c  
    uart.c
    v4l2.c
    ms1100.c
)
add_definitions(-DENABLE_DVPP_INTERFACE)
set(INC_PATH $ENV{DDK_PATH})

if (NOT DEFINED ENV{DDK_PATH})
    set(INC_PATH "/usr/local/Ascend")
    message(STATUS "set default INC_PATH: ${INC_PATH}")
else ()
    message(STATUS "env INC_PATH: ${INC_PATH}")
endif()

set(LIB_PATH $ENV{NPU_HOST_LIB})

if (NOT DEFINED ENV{NPU_HOST_LIB})
    set(LIB_PATH "/usr/local/Ascend/acllib/lib64/stub/")
    message(STATUS "set default LIB_PATH: ${LIB_PATH}")
else ()
    message(STATUS "env LIB_PATH: ${LIB_PATH}")
endif()

include_directories(
    ../inc/
    /home/HwHiAiUser/ascend_ddk/arm/include/opencv4
    /home/HwHiAiUser/ascend_ddk/arm/include/
    ${INC_PATH}/acllib/include/
)

link_directories(
    ../lib
    /usr/local/lib
    ${LIB_PATH}
    /home/HwHiAiUser/ascend_ddk/arm/lib/
    /home/HwHiAiUser/Ascend/
    ${INC_PATH}/atc/lib64
)

add_library(atlas200bsp STATIC ${SRC_LIST})


add_executable(main 
    main.cpp
    model_process.cpp
    object_detect.cpp
    utils.cpp
    yolov3.cpp
    #human_segmentation.cpp
    #classify_process.cpp
)
target_link_libraries(main 
    atlas200bsp 
    pthread 
    ascendcl 
    atlasutil
    #acl_dvpp 
    stdc++ 
    opencv_highgui  
    opencv_core 
    opencv_imgproc 
    opencv_imgcodecs 
    opencv_calib3d 
    opencv_features2d 
    opencv_videoio 
    protobuf 
    presenteragent
    avcodec 
    swresample
)

install(TARGETS main DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

