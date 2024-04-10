#include "./Kvasir.h"

// Control flag to indicate when to stop processing frames
std::atomic<bool> stopProcessing(false);

// Queue to hold frames for processing through AI
std::queue<cv::Mat> frameQueue;

// Mutex to synchronise access to the frame queue
std::mutex queueMutex;

// Condition variable for thread synchronisation for the frame queue
std::condition_variable queueCondVariable;

// FaceStorage object
FaceStorage faceStorage;

// Queue for sharing notification messages to Kvasir Mobile
auto sharedQueue = std::make_shared<NotificationQueue>();

// Face detection component initialisation
FaceDetection faceDetection = FaceDetection();

// Model instance for facial recognition
Model modelInstance(160, sharedQueue);

int main() {
    // Initalise AWS Connection
    S3Communication::initAws();

    // Sync with Kvasir Mobile
    S3Communication::downloadFile("PeopleInformation.json", "../json/faces.json");
    S3Communication::downloadFile("PeopleNotifications.json", "../json/notification.json");

    // Clear output files of previous runs
    Utils::ClearDirectory("../../Kvasir/Components/Output");

    // Load FaceNet model
    modelInstance.LoadModel("../../Kvasir/Components/Models/facenet.tflite");
    modelInstance.BuildInterpreter();

    // Thread for inference
    std::thread processingThread(processFrameInBackground);

    // Thread for processing images from KM
    std::thread imagePollingThread(pollForImages);

    // Thread for pushing notifications
    std::thread notificationThread(notificationDispatcher, sharedQueue);

    // Function to continuously capture
    capture();

    // Tell all processing threads to stop
    stopProcessing = true;

    // Notify threads to exit
    queueCondVariable.notify_one();

    // Wait for all threads to complete execution
    processingThread.join();
    imagePollingThread.join();
    notificationThread.join();

    return 0;
}

void processFrameInBackground() {
    // Continue processing frames
    while (!stopProcessing)
    {
        cv::Mat frame;
        {
            // Lock the queue with a mutex to fore thread safety
            std::unique_lock<std::mutex> lock(queueMutex);

            // Wait for  a new frame to be added to the queue or that processing should stop
            queueCondVariable.wait(lock, []{ return !frameQueue.empty() || stopProcessing; });

            // Break out of the loop if stopProcessing has been set
            if (stopProcessing) break;

            // Retrieve the next frame from the queue
            frame = frameQueue.front();

            // Remove the frame from the queue
            frameQueue.pop();
        }

        // Detect faces in the frame within a confidence threshold
        faceDetection.DetectFaces(frame, 0.5, true);

        // For the number of faces detected within the frame
        for(size_t i = 0; i < faceDetection.GetNumOfFacesDetected(); ++i)
        {
            // Run FaceNet on each cropped face
            modelInstance.HandleImageInput("../../Kvasir/Components/Output/cropped_face_" + std::to_string((i+1)) + ".jpg");
            modelInstance.HandleFaceOutput();
        }
    }
}

void capture() {
    // Open the default camera
    cv::VideoCapture videoCap(0);

    // Create MotionBasedRecorder instance
    MotionBasedRecorder recorder("./", 1, videoCap);

    // Start streaming service
    StreamService::StartStream();

    // Get time of execution
    auto lastTime = std::chrono::high_resolution_clock::now();

    // Capture frames continously
    while (true)
    {
        cv::Mat frame;

        // Store captured frame in variable
        videoCap >> frame;

        // Send the frame to the stream
        StreamService::SendFrame(frame);

        // Send frame to the video file
        recorder.continuousCaptureAndUpload(frame);
        if (frame.empty()) break;

        // Display the frame in a window
        cv::imshow("Frame", frame);
        char key = (char) cv::waitKey(30);
        if (key == 'Q') {
            break;
        }

        // Times to calculate how long its been since last exectuion
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();

        // If its been over 333ms then process next frame
        if (duration > 333)
        {
            // Reset time
            lastTime = currentTime;
            {
                // Lock queue mutex
                std::lock_guard<std::mutex> lock(queueMutex);

                // Add the current frame to the queue for processing
                frameQueue.push(frame.clone());
            }
            // Notify one of the threads that a frame is available
            queueCondVariable.notify_one();
        }
    }
}

void pollForImages() {
    while (!stopProcessing)
    {
        // Check if we should stop processing
        if (stopProcessing) break;

        // Synchronise and process incoming images every minute
        std::this_thread::sleep_for(std::chrono::seconds(15));

        // Check for files in the S3 folder
        std::vector<std::string> fileNames;
        fileNames = S3Communication::getFileNames("TempPersonImage/");

        // If there are files waiting in the S3 to be processed
        if(!fileNames.empty())
        {
            // Synchronise JSON file and wait to for download.
            S3Communication::downloadFile("PeopleInformation.json", "../json/faces.json");
            std::this_thread::sleep_for(std::chrono::seconds(10));

            // For each file in the S3
            for(const auto & i : fileNames)
            {
                // Download the file
                std::string filePath = "../" + i;
                S3Communication::downloadFile(i, filePath);

                // Read the image into a cv::Mat
                cv::Mat image = cv::imread(filePath, cv::IMREAD_COLOR);

                // If faces have been within image - can't be more than 1 face in an image as this is the design
                if(faceDetection.DetectFaceWithinImage(image))
                {
                    std::cout << "Face Detected";
                    // Run FaceNet on cropped image
                    std::vector<float_t> faceData;
                    std::string croppedFilePath = "../TempPersonImage/cropped_face_1.jpg";

                    // Naming formatting fix
                    std::string fileName;
                    std::string prefix = "../TempPersonImage/";

                    // Naming file fixes
                    if (filePath.substr(0, prefix.length()) == prefix)
                    {
                        fileName = filePath.substr(prefix.length());
                    }

                    // Save face data to JSON
                    int digit = fileName[0] - '0';

                    faceData = modelInstance.ProcessImage(croppedFilePath);

                    // Save face to person index of the first character (is a number) within the filename
                    faceStorage.SaveFaceToJSON(digit, faceData);

                    // Delete S3 picture as it has been processed
                    S3Communication::deleteFile(i);

                    // Delete temp files locally
                    //Utils::ClearDirectory("../../Kvasir/TempPersonImage");
                }
                else
                {
                    // If no face was detected delete the image from the S3
                    S3Communication::deleteFile(i);

                    // Delete temp files locally
                    Utils::ClearDirectory("../../Kvasir/TempPersonImage");
                }
            }
        }
    }
}

void notificationDispatcher(const std::shared_ptr<NotificationQueue>& queue) {
    // Struct to hold notification data
    NotificationQueue::Notification notification;
    std::string jsonFilePath = "../json/notification.json";

    // Processor flag
    while (!stopProcessing)
    {
        // Object to R/W JSON file
        nlohmann::json jsonFile;

        // Read JSON file
        std::ifstream jsonFileIn(jsonFilePath);
        if (jsonFileIn.is_open())
        {
            jsonFileIn >> jsonFile;
            jsonFileIn.close();
        }
        else
        {
            std::cout << "error reading JSON file";
        }

        // Wait for a new notification to be added to the queue and pop it
        queue->wait_and_pop(notification);

        // Create a JSON object for the new notification
        nlohmann::json newNotification =
        {
            {"name", notification.name},
            {"reason", notification.reason},
            {"confidence", notification.confidence},
            {"timestamp", notification.timestamp}
        };

        // Push notification to the collection of notifications within the JSON file
        jsonFile["notifications"].push_back(newNotification);

        // Open the JSON file for writing to save the updated notifications
        std::ofstream jsonFileOut(jsonFilePath);

        // Write the updated JSON data to the file
        if (jsonFileOut.is_open())
        {
            jsonFileOut << jsonFile.dump(4);
            jsonFileOut.close();
        }
        else
        {
            std::cerr << "Failed to open " << jsonFilePath << " for writing.\n";
        }

        // update and upload json notification file
        S3Communication::uploadFile("PeopleNotifications.json","../json/notification.json");

        // Wait for a bit to avoid flooding
        std::this_thread::sleep_for(std::chrono::seconds(20));
    }
}