#include <opencv2/opencv.hpp>
#include <string>
class FaceDetection {
public:
    FaceDetection();
    void DetectFaces(cv::Mat &image, float confidenceLevel, bool display = true);
    cv::dnn::Net GetModel();
private:
    cv::dnn::Net faceDetectionModel;
};