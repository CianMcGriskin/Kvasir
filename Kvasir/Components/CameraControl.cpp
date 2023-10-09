#include "CameraControl.h"

void CameraControl::TakePicture(const std::basic_string<char>& filename) {
    cv::VideoCapture cap(0);

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open the camera." << std::endl;
        return;
    }

    std::this_thread::sleep_for(std::chrono::seconds(4));

    cv::Mat frame;
    if (!cap.read(frame))
    {
        std::cerr << "Error: Could not read a frame from the camera." << std::endl;
        return;
    }

    // Save the captured frame as an image file (e.g., "captured_image.jpg")
    cv::imwrite(filename, frame);

    // Release the camera
    cap.release();

    std::cout << "Image captured." << std::endl;
}
