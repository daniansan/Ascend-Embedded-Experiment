#include "human_segmentation.h"

namespace {
const uint32_t kModelWidth = 512;
const uint32_t kModelHeight = 512;
const std::string kModelPath = "../model/human_segmentation.om";
}

int human_segmentation() {
    //init acl resource
    AclDevice aclDev;
    AtlasError ret = aclDev.Init();
    if (ret) {
        ATLAS_LOG_ERROR("Init resource failed, error %d", ret);
        return ATLAS_ERROR;
    }
    //实例化分类推理对象,参数为分类模型路径,模型输入要求的宽和高
    ClassifyProcess classify(kModelPath, kModelWidth, kModelHeight);
    //初始化分类推理的acl资源, 加载模型和申请推理输入使用的内存
    ret = classify.Init();
    if (ret != ATLAS_OK) {
        ATLAS_LOG_ERROR("Classification Init resource failed");
        return ATLAS_ERROR;
    }
    //使用opencv打开视频文件
    cv::VideoCapture capture(0);
    if (!capture.isOpened()) {
        cout << "Movie open Error" << endl;
        return ATLAS_ERROR;
    }
    //逐帧推理
    while(1) {
        //读取一帧图片
        cv::Mat frame;
        if (!capture.read(frame))
        {
            ATLAS_LOG_INFO("Video capture return false");
            break;
        }
        //对帧图片进行预处理
        AtlasError ret = classify.Preprocess(frame);
        if (ret != ATLAS_OK) {
            continue;
        }
        //将预处理的图片送入模型推理,并获取推理结果
        std::vector<InferenceOutput> inferOutputs;
        ret = classify.Inference(inferOutputs);
        if (ret != ATLAS_OK) {
            ATLAS_LOG_ERROR("Inference model inference output data failed");
            return ATLAS_ERROR;
        }

        //解析推理输出,并将推理得到的物体类别,置信度和图片送到presenter server显示
        ret = classify.Postprocess(frame, inferOutputs);
        if (ret != ATLAS_OK) {
            ATLAS_LOG_ERROR("Process model inference output data failed");
            return ATLAS_ERROR;
        }
    }

    ATLAS_LOG_INFO("Execute video classification success");
    return ATLAS_OK;
}
