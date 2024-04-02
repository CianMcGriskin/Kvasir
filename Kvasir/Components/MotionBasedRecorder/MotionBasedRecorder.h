#ifndef KVASIR_MOTIONBASEDRECORDER_H
#define KVASIR_MOTIONBASEDRECORDER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <thread>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include "../WebserverCommunication/S3Communication.h"

class MotionBasedRecorder {
public:
    // Constructor
    MotionBasedRecorder(std::string outputDir, uint16_t segmentLength, const cv::VideoCapture& videoCap);

    // Continuously captures video frames, detects motion, and handles video recording and uploading
    void continuousCaptureAndUpload(const cv::Mat& frame);

private:
    // Video capture device
    cv::VideoCapture videoCapture;

    // Directory for storing video segments
    std::string outputDirectory;

    // Flag to indicate if recording is currently active
    bool isRecording;

    // Time when the current video segment started
    std::chrono::steady_clock::time_point segmentStartTime;

    // Last time motion was detected
    std::chrono::steady_clock::time_point lastMotionDetectedTime;

    // OpenCV video writer for recording segments
    cv::VideoWriter videoWriter;

    // Length of each video segment in minutes
    int16_t segmentLength;

    // Name of the current video segment file
    std::string segmentFileName;

    // Previous frame, used for motion detection
    cv::Mat prevFrame;

    // Finalises the current video segment and uploads it to S3
    void stopRecordingAndUpload(const std::string& fileName);

    // Starts a new video segment and returns the file name of the segment
    std::string startNewSegment();

    // Appends a video frame to the current segment
    void appendFrameToSegment(const cv::Mat& frame, const std::string& fileName);
};

#endif //KVASIR_MOTIONBASEDRECORDER_H