#include "yolov3.h"

namespace {
uint32_t kModelWidth = 416;
uint32_t kModelHeight = 416;
const char* kModelPath = "../model/yolov3.om";
}

int Yolov3_Test() {
    
    //Instantiate the target detection class with the parameters of the classification model path and the required width and height of the model input
    ObjectDetect detect(kModelPath, kModelWidth, kModelHeight);
    //Initializes the ACL resource for categorical reasoning, loads the model and requests the memory used for reasoning input
    Result ret = detect.Init();
    if (ret != SUCCESS) {
        ERROR_LOG("Classification Init resource failed");
        return FAILED;
    }
    //Use Opencv to open the video file
    string videoFile = string("./video.mp4");
    cv::VideoCapture capture(videoFile);
    if (!capture.isOpened()) {
        ERROR_LOG("Movie open Erro");
        return FAILED;
    }
    //Frame by frame reasoning
    while(1) {
        //Read a frame of an image
        cv::Mat frame;
        if (!capture.read(frame)) {
            INFO_LOG("Video capture return false");
            break;
        }
        //The frame image is preprocessed
        Result ret = detect.Preprocess(frame);
        if (ret != SUCCESS) {
            ERROR_LOG("Read file %s failed, continue to read next",
                      videoFile.c_str());
            continue;
        }
        //The preprocessed images are fed into model reasoning and the reasoning results are obtained
        aclmdlDataset* inferenceOutput = nullptr;
        ret = detect.Inference(inferenceOutput);
        if ((ret != SUCCESS) || (inferenceOutput == nullptr)) {
            ERROR_LOG("Inference model inference output data failed");
            return FAILED;
        }
        //Parses the inference output and sends the inference class, location, confidence, and image to the Presenter Server for display
        ret = detect.Postprocess(frame, inferenceOutput);
        if (ret != SUCCESS) {
            ERROR_LOG("Process model inference output data failed");
            return FAILED;
        }
    }

    INFO_LOG("Execute video object detection success");
    return SUCCESS;
}
