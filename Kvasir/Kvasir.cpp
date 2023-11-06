﻿#include "./Kvasir.h"

int main() {
    // Load the model using model class
    Model modelInstance(160);
    CameraControl camera;
//    cv::VideoCapture cap(0);
//    cv::Mat frame;

    // Video input and resolution size

    camera.TakePicture("../../Kvasir/capture.jpg");
    modelInstance.LoadModel("../../Kvasir/facenet.tflite");
    modelInstance.BuildInterpreter();

    modelInstance.HandleImageInput("../../Kvasir/capture.jpg");
    modelInstance.HandleFaceOutput();

    cv::imshow("Detection Results", modelInstance.GetInput());
    cv::waitKey(0);
//    StreamService::StartStream();
//    // Main loop for processing frames from the camera
//   while (true) {
//       // Capture a frame from the camera
//       cap >> frame;
//
//       StreamService::SendFrame(frame);
//
//       // Call the HandleInput function to preprocess and run inference
//       modelInstance.HandleInput(320, frame);
//       modelInstance.HandleOutput(0.5);
//
//       // Display the original frame (optional: display the processed frame with inference result)
//       cv::imshow("Camera Feed", modelInstance.GetInput());
//
//       // Break the loop if 'q' is pressed
//       if (cv::waitKey(1) == 'q') {
//           break;
//       }
//   }

    // close OpenCV windows
    cv::destroyAllWindows();
//    StreamService::StopStream();

    return 0;
}
