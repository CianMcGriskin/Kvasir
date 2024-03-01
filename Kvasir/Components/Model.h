#include <tensorflow/lite/model.h>
#include "tensorflow/lite/core/kernels/register.h"
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "FaceDetection/FaceDetection.h"
#include "NotificationQueue/NotificationQueue.h"
#include <chrono>
#include "FaceStorage/FaceStorage.h"

class Model {
public:
    // Constructor for the model class, assigned model LxW needed
    Model(int16_t modelSize, const std::shared_ptr<NotificationQueue>& notificationQueue);

    // Function used to load model from File
    void LoadModel(const char* modelPath);

    // Function to build interpreter - driver class to run pre-trained models
    void BuildInterpreter();

    // Handles input coming from an image, used mainly for development purposes
    void HandleImageInput(const std::string& imagePath);

    // Function that returns the model
    std::unique_ptr<tflite::FlatBufferModel>& GetModel();

    std::vector<float_t> ProcessImage(const std::string& imagePath);

    void HandleFaceOutput();

    cv::Mat GetInput();

private:
    int16_t modelParam;
    std::vector<std::string> classLabels;
    std::unique_ptr<tflite::FlatBufferModel> model;
    std::unique_ptr<tflite::Interpreter> interpreter;
    cv::Mat input;
    float* output;
    std::vector<float> faceEmbeddings;
    std::shared_ptr<NotificationQueue> notificationQueue;

    void DrawBox(int maxConfidenceIndex, float maxConfidence, int numValuesPerDetection);
};