#include "./Kvasir.h"
#include "Components/MotionBasedRecorder/MotionBasedRecorder.h"

void capture();
void processFrameInBackground();
void pollForImages();
void notificationDispatcher(const std::shared_ptr<NotificationQueue>& queue);

std::atomic<bool> stopProcessing(false);
std::queue<cv::Mat> frameQueue;
std::mutex queueMutex;
std::condition_variable queueCondVariable;
FaceStorage faceStorage;

auto sharedQueue = std::make_shared<NotificationQueue>();
Model modelInstance(160, sharedQueue);
FaceDetection faceDetection = FaceDetection();

int main() {
    S3Communication::initAws();

    S3Communication::downloadFile("PeopleInformation.json", "../json/faces.json");
    S3Communication::downloadFile("PeopleNotifications.json", "../json/notification.json");

    Utils::ClearDirectory("../../Kvasir/Components/Output");

    modelInstance.LoadModel("../../Kvasir/Components/Models/facenet.tflite");
    modelInstance.BuildInterpreter();

    std::thread processingThread(processFrameInBackground);
    std::thread imagePollingThread(pollForImages);
    std::thread notificationThread(notificationDispatcher, sharedQueue);

    capture();

    stopProcessing = true;
    queueCondVariable.notify_one();

    processingThread.join();
    imagePollingThread.join();
    notificationThread.join();

    return 0;
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

        if(!fileNames.empty())
        {
            // Synchronise JSON file and wait to for download.
            S3Communication::downloadFile("PeopleInformation.json", "../json/faces.json");
            std::this_thread::sleep_for(std::chrono::seconds(10));

            for(const auto & i : fileNames)
            {
                std::string filePath = "../" + i;
                S3Communication::downloadFile(i, filePath);


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
                S3Communication::deleteFile(i);

                // Delete temp files locally
                Utils::ClearDirectory("../../Kvasir/TempPersonImage");
            }
        }
    }
}

void notificationDispatcher(const std::shared_ptr<NotificationQueue>& queue) {
    NotificationQueue::Notification notification;
    std::string jsonFilePath = "../json/notification.json";

    while (!stopProcessing)
    {
        nlohmann::json jsonFile;

        std::ifstream jsonFileIn(jsonFilePath);
        if (jsonFileIn.is_open())
        {
            jsonFileIn >> jsonFile;
            jsonFileIn.close();
        }
        else
        {
            std::cout << "error";
        }

        queue->wait_and_pop(notification);

        nlohmann::json newNotification =
        {
            {"name", notification.name},
            {"reason", notification.reason},
            {"confidence", notification.confidence},
            {"timestamp", notification.timestamp}
        };

        jsonFile["notifications"].push_back(newNotification);


        std::ofstream jsonFileOut(jsonFilePath);
        if (jsonFileOut.is_open()) {
            jsonFileOut << jsonFile.dump(4); // Pretty print with 4 spaces indent
            jsonFileOut.close();
        } else {
            std::cerr << "Failed to open " << jsonFilePath << " for writing.\n";
        }

        // update and upload json notification file
        S3Communication::uploadFile("PeopleNotifications.json","../json/notification.json");

        // Wait for a bit to avoid flooding
        std::this_thread::sleep_for(std::chrono::seconds(25));
    }
}