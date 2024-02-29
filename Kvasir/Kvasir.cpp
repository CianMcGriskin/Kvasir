#include "./Kvasir.h"
#include "Components/MotionBasedRecorder/MotionBasedRecorder.h"

void capture();
void processFrameInBackground();
void pollForImages();

std::atomic<bool> stopProcessing(false);
std::queue<cv::Mat> frameQueue;
std::mutex queueMutex;
std::condition_variable queueCondVariable;
FaceStorage faceStorage;

Model modelInstance(160);
FaceDetection faceDetection = FaceDetection();

int main() {
    S3Communication::initAws();

    Utils::ClearDirectory("../../Kvasir/Components/Output");

    modelInstance.LoadModel("../../Kvasir/Components/Models/facenet.tflite");
    modelInstance.BuildInterpreter();

    std::thread processingThread(processFrameInBackground);
    std::thread imagePollingThread(pollForImages);

    capture();

    stopProcessing = true;
    queueCondVariable.notify_one();

    processingThread.join();
    imagePollingThread.join();

    return 0;

//    cv::dnn::Net faceDetectionModel = faceDetection.GetModel();


//    // If face was detected within a frame
//    if (faceDetection.GetNumOfFacesDetected() > 0)
//    {
//
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

void processFrameInBackground() {
    while (!stopProcessing)
    {
        cv::Mat frame;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondVariable.wait(lock, []{ return !frameQueue.empty() || stopProcessing; });

            if (stopProcessing) break;

            frame = frameQueue.front();
            frameQueue.pop();
        }

        faceDetection.DetectFaces(frame, 0.5, true);

        for(size_t i = 0; i < faceDetection.GetNumOfFacesDetected(); ++i)
        {
            modelInstance.HandleImageInput("../../Kvasir/Components/Output/cropped_face_" + std::to_string((i+1)) + ".jpg");
            modelInstance.HandleFaceOutput();
        }
    }
}

void capture() {
    // Open the default camera
    cv::VideoCapture videoCap(0);

    MotionBasedRecorder Object("./", 1, videoCap);
    StreamService::StartStream();

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (true)
    {
        cv::Mat frame;

        videoCap >> frame;
        StreamService::SendFrame(frame);

        Object.continuousCaptureAndUpload(frame);
        if (frame.empty()) break;

        cv::imshow("Frame", frame);
        if (cv::waitKey(30) >= 0) break;

        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();

        if (duration > 333)
        {
            lastTime = currentTime;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                frameQueue.push(frame.clone());
            }
            queueCondVariable.notify_one();
        }
    }
}

void pollForImages() {
    while (!stopProcessing) {

        // Check if we should stop processing
        if (stopProcessing) break;

        // Synchronise and process every minute
        std::this_thread::sleep_for(std::chrono::minutes(1));

        // Check for files in the S3 folder
        std::vector<std::string> fileNames;
        fileNames = S3Communication::getFileNames("TempPersonImage/");

        if(fileNames.size() > 0)
        {
            // Synchronise JSON file and wait to for download.
            S3Communication::downloadFile("PeopleInformation.json", "../json/faces.json");
            std::this_thread::sleep_for(std::chrono::seconds(10));

            for(size_t i = 0; i < fileNames.size(); ++i)
            {
                std::string filePath = "../" + fileNames[i];
                S3Communication::downloadFile(fileNames[i], filePath);


                cv::Mat image = cv::imread(filePath, cv::IMREAD_COLOR);

                // Detect faces within image - can't be more than 1 face in an image as this is the design
                faceDetection.DetectFaceWithinImage(image);

                // Run model to get
                std::vector<float_t> faceData;
                faceData = modelInstance.ProcessImage(filePath);

                // Naming formatting fix
                std::string fileName;
                std::string prefix = "../TempPersonImage/";
                if (filePath.substr(0, prefix.length()) == prefix)
                {
                    fileName = filePath.substr(prefix.length());
                }

                // Save face data to JSON
                int digit = fileName[0] - '0';
                faceStorage.SaveFaceToJSON(digit, faceData);

                // Delete S3 picture as it has been processed
                S3Communication::deleteFile(fileNames[i]);

                // Delete temp files locally
                Utils::ClearDirectory("../../Kvasir/TempPersonImage");
            }
        }
    }
}