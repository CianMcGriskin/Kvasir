#include "CameraControl.h"

// Used to take a picture using a camera and saving it to a file located where the param is
void CameraControl::TakePicture(const std::basic_string<char>& filename) {
    camera.open(0);

    if (!camera.isOpened())
    {
        std::cerr << "Error: Could not open the camera." << std::endl;
        return;
    }

    // Attempting to fix issue of camera quality being bad on startup, doesn't seem to work
    std::this_thread::sleep_for(std::chrono::seconds(5));

    if (!camera.read(frame))
    {
        std::cerr << "Error: Could not read a frame from the camera." << std::endl;
        return;
    }

    // Save the captured frame as an image file
    cv::imwrite(filename, frame);

    // Release the camera
    camera.release();

    std::cout << "Image captured." << std::endl;
}


void CameraControl::StartCamera(){
    camera.open(0);

    if (!camera.isOpened())
    {
        std::cerr << "Error: Could not open the camera." << std::endl;
        return;
    }

    while (true)
    {
        // Capture frame from the camera
        camera >> frame;

        // Check if the frame is empty
        if (frame.empty())
        {
            std::cerr << "Error: Could not read frame from the camera." << std::endl;
            break;
        }

        // Break the loop if 'q' is pressed
        if (cv::waitKey(1) == 'q')
        {
            break;
        }
    }
}

cv::Mat CameraControl::GetCurrentFrame(){
    return frame;
}