#ifndef KVASIR_MOTIONBASEDRECORDER_H
#define KVASIR_MOTIONBASEDRECORDER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include "../WebserverCommunication/S3Communication.h"

class MotionBasedRecorder {
public:
    MotionBasedRecorder(std::string outputDir, std::string bucketName, uint16_t segmentLength);
    void start();

private:
    cv::VideoCapture videoCapture;
    std::string outputDirectory;
    std::string s3BucketName;
    bool isRecording;
    std::chrono::steady_clock::time_point lastMotionDetectedTime;
    uint16_t segmentLength;
    cv::VideoWriter videoWriter;

    // Finalises the current video segment and uploads it to S3
    void stopRecordingAndUpload(const std::string& fileName);

    // Sets up the video capture device - To be changed
    void setupVideoCapture();

    // Continuously captures video frames, detects motion, and handles video recording and uploading
    void continuousCaptureAndUpload();

    // Starts a new video segment and returns the file name of the segment
    std::string startNewSegment();

    // Appends a video frame to the current segment
    void appendFrameToSegment(const cv::Mat& frame, const std::string& fileName);
};

#endif //KVASIR_MOTIONBASEDRECORDER_H
