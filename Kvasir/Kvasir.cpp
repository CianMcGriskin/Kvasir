#include "./Kvasir.h"

int main(){
    S3Communication::initAws();
    S3Communication::readJsonFile("../json/facesV2.json");
    S3Communication::shutdownAWS();
    // Load the model using model class
    Utils::ClearDirectory("../../Kvasir/Components/Output");

    CameraControl camera;
    FaceDetection faceDetection = FaceDetection();
//    cv::VideoCapture cap(0);
//    cv::Mat frame;

    // Video input and resolution size
    camera.TakePicture("../../Kvasir/capture.jpg");
    cv::Mat image = cv::imread("../../Kvasir/capture.jpg");

    cv::dnn::Net faceDetectionModel = faceDetection.GetModel();
    faceDetection.DetectFaces(image, 0.5);

    Model modelInstance(160);
    modelInstance.LoadModel("../../Kvasir/Components/Models/facenet.tflite");
    modelInstance.BuildInterpreter();

    // If face was detected within a frame
    if (faceDetection.GetNumOfFacesDetected() > 0)
    {
        // Loop through faces found in current frame
        for(size_t i = 0; i < faceDetection.GetNumOfFacesDetected(); ++i)
        {
            modelInstance.HandleImageInput("../../Kvasir/Components/Output/cropped_face_" + std::to_string((i+1)) + ".jpg");

            modelInstance.HandleFaceOutput();

            std::cout << "Do you want to save this face? (y/n): ";
            char choice;
            std::cin >> choice;
            if (choice == 'y' || choice == 'Y')
            {
                FaceStorage faceStorage = FaceStorage();
                faceStorage.SaveFaceToJSON(modelInstance.GetFaceEmbeddings());
                std::cout << "Face saved successfully!" << std::endl;
            }
            else
            {
                std::cout << "Face not saved." << std::endl;
            }
        }
    } else {
        std::cout << "No faces detected.";
    }

//    cv::imshow("Detection Results", modelInstance.GetInput());
//    cv::waitKey(0);
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
//    cv::destroyAllWindows();
//    StreamService::StopStream();
        return 0;
}