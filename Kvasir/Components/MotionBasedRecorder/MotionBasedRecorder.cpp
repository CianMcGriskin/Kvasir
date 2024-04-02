#include "MotionBasedRecorder.h"

MotionBasedRecorder::MotionBasedRecorder(std::string outputDir, uint16_t segmentLength, const cv::VideoCapture& videoCap)
    : outputDirectory(std::move(outputDir)), isRecording(false), segmentLength(segmentLength), videoCapture(videoCap)
    {}

void MotionBasedRecorder::continuousCaptureAndUpload(const cv::Mat& frame) {
    // Frame storage for grayscale
    cv::Mat gray;

    // Convert captured frame to grayscale and apply Gaussian blur for motion detection
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(21, 21), 0);

    if (!prevFrame.empty())
    {
        // Calculate the difference between the current and previous frames
        cv::Mat frameDelta, thresh;
        cv::absdiff(prevFrame, gray, frameDelta);
        cv::threshold(frameDelta, thresh, 25, 255, cv::THRESH_BINARY);

        // Detect contours in the frame to find areas of motion
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        auto now = std::chrono::steady_clock::now();

        // Motion is detected if contours are found
        bool motionDetected = !contours.empty();
        bool noMotionTimeout = isRecording && (now - lastMotionDetectedTime > std::chrono::seconds(3));

        // Check if the maximum duration for the current segment has been reached
        bool segmentMaxDurationReached = (now - segmentStartTime) > std::chrono::minutes(segmentLength);

        // If motion detected
        if (motionDetected)
        {
            // If not recording or reached max segment time
            if (!isRecording || segmentMaxDurationReached)
            {
                // If segment time reached and is still recording
                if (isRecording)
                {
                    // Stop recording and upload segment
                    std::cout << "Finalising current segment due to max duration." << std::endl;
                    stopRecordingAndUpload(segmentFileName);
                }

                // Start recording new segment
                segmentFileName = startNewSegment();
                isRecording = true;
                segmentStartTime = std::chrono::steady_clock::now();
                std::cout << "Started new segment: " << segmentFileName << std::endl;
            }

            // Update the time when the last motion was detected
            lastMotionDetectedTime = now;
        }
        // If no motion has been recorded for the last number of timeout seconds
        else if (noMotionTimeout)
        {
            // Stop recording and upload segment
            std::cout << "No motion detected for 10 seconds, stopping recording." << std::endl;
            stopRecordingAndUpload(segmentFileName);
            isRecording = false;
            segmentFileName.clear();
        }

        // If recording is active, append the current frame to the segment
        if (isRecording)
        {
            appendFrameToSegment(frame, segmentFileName);
        }
    }

    // Update prevFrame with the current frame for the next iteration
    prevFrame = gray.clone();
}

void MotionBasedRecorder::stopRecordingAndUpload(const std::string& fileName) {
    // Check that file exists
    if (!fileName.empty())
    {
        // If videoWriter is still opened then realese is, this is to avoid uploading an unfinished video
        if (videoWriter.isOpened())
        {
            videoWriter.release();
        }

        // Check upload was successful
        bool uploadSuccess = S3Communication::uploadVideoSegment(fileName);

        // Remove local video after upload to cloud successful
        if (uploadSuccess)
        {
            // Only remove the file if upload was successful
            std::remove(fileName.c_str());
            std::cout << "File uploaded successfully and removed locally: " << fileName << std::endl;
        }
        else
        {
            // Handle upload failure
            std::cerr << "Failed to upload file: " << fileName << std::endl;
        }
    }
    std::cout << "Segment stopped and uploaded: " << fileName << std::endl;
}

// Function to begin a new video segment
std::string MotionBasedRecorder::startNewSegment() {
    // Generate new segment file name based on timestamp
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::string fileName = outputDirectory + "segment_" + std::to_string(now_c) + ".mp4";

    // Frame size from the video capture
    int frameWidth = static_cast<int>(videoCapture.get(3));
    int frameHeight = static_cast<int>(videoCapture.get(4));
    cv::Size frameSize(frameWidth, frameHeight);

    // Define the codec and initialise videoWriter
    int codec = cv::VideoWriter::fourcc('a', 'v', 'c', '1');
    double fps = 10;

    // Open video file for writing
    videoWriter.open(fileName, codec, fps, frameSize);

    if (!videoWriter.isOpened())
    {
        std::cerr << "Could not open the video file for write: " << fileName << std::endl;
        return "";
    }

    return fileName;
}

// Add captured frame to video file
void MotionBasedRecorder::appendFrameToSegment(const cv::Mat& frame, const std::string& fileName) {
    // If video writer is opened then append frame to segment video
    if (videoWriter.isOpened())
    {
        videoWriter.write(frame);
    }
    else
    {
        std::cerr << "VideoWriter is not opened for file: " << fileName << std::endl;
    }
}