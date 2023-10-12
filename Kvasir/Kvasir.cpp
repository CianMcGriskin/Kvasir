#include "./Components/CameraControl.h"
#include "./Components/CameraConnection.h"
#include "./Components/Model.h"
#include <opencv2/opencv.hpp>

int main() {
    // Load the model using model class
    Model modelInstance(320);
    CameraControl camera;
    CameraConnection camera1;
    camera1.OpenCamera();
    cv::VideoCapture cap(0);

    //camera.TakePicture("../../Kvasir/capture.jpg");

    modelInstance.LoadModel("../../Kvasir/yolov5.tflite");

    modelInstance.BuildInterpreter();
    //modelInstance.HandleInput(320, "../../Kvasir/capture.jpg");

    // Main loop for processing frames from the camera
    while (true) {
        cv::Mat frame;
        // Capture a frame from the camera
        cap >> frame;

        // Call the HandleInput function to preprocess and run inference
        modelInstance.HandleInput(320, frame);
        modelInstance.HandleOutput(0.5);
        // Display the original frame (optional: display the processed frame with inference result)
        cv::imshow("Camera Feed", frame);

        // Break the loop if 'q' is pressed
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    // Release the camera capture and close OpenCV windows
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
