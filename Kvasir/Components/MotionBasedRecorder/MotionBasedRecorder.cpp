#include "MotionBasedRecorder.h"

#include <utility>

MotionBasedRecorder::MotionBasedRecorder(std::string outputDir, std::string bucketName, uint16_t segmentLength)
    : outputDirectory(std::move(outputDir)), s3BucketName(std::move(bucketName)), isRecording(false), segmentLength(segmentLength)
    {}

void MotionBasedRecorder::start() {
    setupVideoCapture();
    continuousCaptureAndUpload();
}

void MotionBasedRecorder::setupVideoCapture() {
    // Open default camera
    videoCapture.open(0);

    if (!videoCapture.isOpened())
    {
        std::cerr << "Error: Cannot open video capture." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void MotionBasedRecorder::continuousCaptureAndUpload() {
    // Frame storage for current, grayscale, and previous frames
    cv::Mat frame, gray, prevFrame;
    std::string segmentFileName;
    auto segmentStartTime = std::chrono::steady_clock::now();

    while (true) {
        videoCapture >> frame;
        if (frame.empty()) break;

        // Convert captured frame to grayscale and apply Gaussian blur for motion detection
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, gray, cv::Size(21, 21), 0);

        if (!prevFrame.empty())
        {
            // Calculate the difference between the current and previous frames
            cv::Mat frameDelta, thresh;
            cv::absdiff(prevFrame, gray, frameDelta);
            cv::threshold(frameDelta, thresh, 25, 255, cv::THRESH_BINARY);

            // Detect contours in the thresholded frame to find areas of motion
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            auto now = std::chrono::steady_clock::now();

            // Motion is detected if contours are found
            bool motionDetected = !contours.empty();
            bool noMotionTimeout = isRecording && (now - lastMotionDetectedTime > std::chrono::seconds(10));

            // Check if the maximum duration for the current segment has been reached
            bool segmentMaxDurationReached = (now - segmentStartTime) > std::chrono::minutes(segmentLength);

            // Start a new segment if motion is detected or the current segment has reached its maximum duration
            if (motionDetected)
            {
                if (!isRecording || segmentMaxDurationReached)
                {
                    if (isRecording)
                    {
                        // Finalize the current segment before starting a new one
                        stopRecordingAndUpload(segmentFileName);
                    }
                    segmentFileName = startNewSegment();
                    isRecording = true;
                    segmentStartTime = std::chrono::steady_clock::now();
                }

                // Update the time when the last motion was detected
                lastMotionDetectedTime = now;
            }
            else if (noMotionTimeout)
            {
                // If no motion is detected for 10 seconds, finalize and upload the current segment
                stopRecordingAndUpload(segmentFileName);
                isRecording = false;
            }

            // If recording is active, append the current frame to the segment
            if (isRecording)
            {
                appendFrameToSegment(frame, segmentFileName);
            }
        }

        prevFrame = gray.clone();
    }
}

void MotionBasedRecorder::stopRecordingAndUpload(const std::string& fileName) {
    if (videoWriter.isOpened())
    {
        videoWriter.release();
    }

    if (!fileName.empty())
    {
        S3Communication::uploadVideoSegment(fileName);

        // Remove local video after upload to cloud successful
        std::remove(fileName.c_str());
    }
    std::cout << "Segment stopped and uploaded: " << fileName << std::endl;
}

std::string MotionBasedRecorder::startNewSegment() {
    // Ensure any existing videoWriter is properly closed before starting a new one
    if (videoWriter.isOpened())
    {
        videoWriter.release();
    }

    // Generate new segment file name based on timestamp
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::string fileName = outputDirectory + "/segment_" + std::to_string(now_c) + ".mp4";

    // Define the codec and initialize videoWriter
    int codec = cv::VideoWriter::fourcc('H', '2', '6', '4');
    float fps = 30.0;
    cv::Size frameSize(1280, 720); // Assuming 720p resolution

    videoWriter.open(fileName, codec, fps, frameSize, true);

    if (!videoWriter.isOpened())
    {
        std::cerr << "Could not open the video file for write: " << fileName << std::endl;
        // Handle the error appropriately - you could throw an exception or return an empty string
        return "";
    }

    return fileName;
}

void MotionBasedRecorder::appendFrameToSegment(const cv::Mat& frame, const std::string& fileName) {
    if (videoWriter.isOpened())
    {
        videoWriter.write(frame);
    } else {
        std::cerr << "VideoWriter is not opened for file: " << fileName << std::endl;
    }
}
