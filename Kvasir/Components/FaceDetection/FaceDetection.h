#ifndef FACE_DETECTION_H
#define FACE_DETECTION_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>

class FaceDetection {
public:
    FaceDetection();
    void DetectFaces(cv::Mat &image, float confidenceLevel, bool display = true);
    static double CompareFaces(std::vector<double> vec, short size = 512);
    size_t GetNumOfFacesDetected();
    cv::dnn::Net GetModel();
private:
    static unsigned long numOfFacesDetected;
    cv::dnn::Net faceDetectionModel;
};

#endif /* FACE_DETECTION_H */