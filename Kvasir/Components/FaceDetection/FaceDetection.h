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

    // Function that detects faces within live video and saves cropped images of faces detected
    void DetectFaces(cv::Mat &image, float confidenceLevel, bool display = true);

    // Function to compare two vectors of faces for similarity between face data
    static float CompareFaces(std::vector<float> currentFace, std::vector<float> savedFace, short size = 512);

    // Function that returns number of faces detected within a frame
    size_t GetNumOfFacesDetected();

    // Function to detect faces within an image sent from Kvasir Mobile
    bool DetectFaceWithinImage(cv::Mat &image, float confidenceLevel = 0.5);

    // Function that returns Caffe Model
    cv::dnn::Net GetModel();
private:
    // Variable to hold number of faces detected within an image
    static unsigned char numOfFacesDetected;

    // Variable to hold Caffe Model
    cv::dnn::Net faceDetectionModel;
};

#endif /* FACE_DETECTION_H */