#include "./Kvasir.h"
#include "Components/MotionBasedRecorder/MotionBasedRecorder.h"

void capture();

int main() {
    S3Communication::initAws();
    S3Communication::readJsonFile("../json/faces.json");

    capture();


//    // Load the model using model class
//    Utils::ClearDirectory("../../Kvasir/Components/Output");
//
//    CameraControl camera;
//    FaceDetection faceDetection = FaceDetection();
////    cv::VideoCapture cap(0);
////    cv::Mat frame;
//
//    // Video input and resolution size
//    camera.TakePicture("../../Kvasir/capture.jpg");
//    cv::Mat image = cv::imread("../../Kvasir/capture.jpg");
//
//    cv::dnn::Net faceDetectionModel = faceDetection.GetModel();
//    faceDetection.DetectFaces(image, 0.5);
//
//    Model modelInstance(160);
//    modelInstance.LoadModel("../../Kvasir/Components/Models/facenet.tflite");
//    modelInstance.BuildInterpreter();
//
//    // If face was detected within a frame
//    if (faceDetection.GetNumOfFacesDetected() > 0)
//    {
//        FaceStorage faceStorage = FaceStorage();
//        faceStorage.GetJsonData();
//
//        // Loop through faces found in current frame
//        for(size_t i = 0; i < faceDetection.GetNumOfFacesDetected(); ++i)
//        {
//            modelInstance.HandleImageInput("../../Kvasir/Components/Output/cropped_face_" + std::to_string((i+1)) + ".jpg");
//
//            modelInstance.HandleFaceOutput();
//
//            std::cout << "\nDo you want to save this face? (y/n): ";
//            char choice;
//            std::cin >> choice;
//            if (choice == 'y' || choice == 'Y')
//            {
//                int personIndex;
//                std::cout << "Enter the person index for this face (-1 for a new person): ";
//                std::cin >> personIndex;
//                faceStorage.SaveFaceToJSON(personIndex, modelInstance.GetFaceEmbeddings());
//                std::cout << "Face saved successfully under person index " << personIndex << std::endl;
//            }
//            else
//            {
//                std::cout << "Face not saved." << std::endl;
//            }
//        }
//    }
//    S3Communication::shutdownAWS();
//        return 0;
}

void capture() {
    // Open the default camera
    cv::VideoCapture videoCap(0);

    MotionBasedRecorder Object("./", 1, videoCap);

    while (true) {
        cv::Mat frame;

        videoCap >> frame;

        Object.continuousCaptureAndUpload(frame);
        if (frame.empty()) break;
        
        cv::imshow("Frame", frame);
        if (cv::waitKey(30) >= 0) break; // Break on any key press
    }
}