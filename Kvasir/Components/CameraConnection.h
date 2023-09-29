#include <opencv2/opencv.hpp>

class CameraConnection {
public:
    // Open the camera
    void OpenCamera();

    // Close the camera and window
    void CloseCamera();

private:
    // Camera object
    cv::VideoCapture camera;
};
