#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>

class CameraControl {
public:
    // Function used to take a picture of the current camera frame
    void TakePicture(const std::basic_string<char>& filename);
private:

};
