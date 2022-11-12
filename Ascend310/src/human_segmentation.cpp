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
    //ʵ���������������,����Ϊ����ģ��·��,ģ������Ҫ��Ŀ�͸�
    ClassifyProcess classify(kModelPath, kModelWidth, kModelHeight);
    //��ʼ�����������acl��Դ, ����ģ�ͺ�������������ʹ�õ��ڴ�
    ret = classify.Init();
    if (ret != ATLAS_OK) {
        ATLAS_LOG_ERROR("Classification Init resource failed");
        return ATLAS_ERROR;
    }
    //ʹ��opencv����Ƶ�ļ�
    cv::VideoCapture capture(0);
    if (!capture.isOpened()) {
        cout << "Movie open Error" << endl;
        return ATLAS_ERROR;
    }
    //��֡����
    while(1) {
        //��ȡһ֡ͼƬ
        cv::Mat frame;
        if (!capture.read(frame))
        {
            ATLAS_LOG_INFO("Video capture return false");
            break;
        }
        //��֡ͼƬ����Ԥ����
        AtlasError ret = classify.Preprocess(frame);
        if (ret != ATLAS_OK) {
            continue;
        }
        //��Ԥ�����ͼƬ����ģ������,����ȡ������
        std::vector<InferenceOutput> inferOutputs;
        ret = classify.Inference(inferOutputs);
        if (ret != ATLAS_OK) {
            ATLAS_LOG_ERROR("Inference model inference output data failed");
            return ATLAS_ERROR;
        }

        //�����������,��������õ����������,���ŶȺ�ͼƬ�͵�presenter server��ʾ
        ret = classify.Postprocess(frame, inferOutputs);
        if (ret != ATLAS_OK) {
            ATLAS_LOG_ERROR("Process model inference output data failed");
            return ATLAS_ERROR;
        }
    }

    ATLAS_LOG_INFO("Execute video classification success");
    return ATLAS_OK;
}
