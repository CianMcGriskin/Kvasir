#include "CameraConnection.h"

// Camera object
cv::VideoCapture camera;

void CameraConnection::OpenCamera() {
    // Connect to device camera (default camera is index 0)
    camera.open(0);

    if (!camera.isOpened()) 
    {
        std::cerr << "Error: Could not open the camera." << std::endl;
    }
    else 
    {
        // Create a window for displaying the camera feed
        cv::namedWindow("Camera", cv::WINDOW_NORMAL);

        while (true) {
            cv::Mat frame;
            camera >> frame;

            if (frame.empty()) {
                std::cerr << "Error: Capture failed." << std::endl;
                break;
            }

            cv::imshow("Camera", frame);

            int key = cv::waitKey(30);
            if (key != -1) 
            {
                CameraConnection::CloseCamera();
            }
        }
    }
}

void CameraConnection::CloseCamera() {
    if (camera.isOpened()) {
        camera.release();
    }

    cv::destroyAllWindows();
}