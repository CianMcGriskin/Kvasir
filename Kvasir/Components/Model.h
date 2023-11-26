#include <tensorflow/lite/model.h>
#include "tensorflow/lite/core/kernels/register.h"
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "FaceDetection/FaceDetection.h"
#include <chrono>
#include "FaceStorage/FaceStorage.h"

class Model {
public:
    // Constructor for the model class assigned model LxW needed
    Model(int16_t modelSize);

    // Function used to load model from File
    void LoadModel(const char* modelPath);

    // Function used to load label identifiers
    std::vector<std::string> LoadIdentityLabels(const std::string& labelsFilePath);

    // Function to build interpreter - driver class to run pre-trained models
    void BuildInterpreter();

    // Handles the input from a live frame, conversions to fit model criteria
    void HandleInput(int16_t modelSize, const cv::Mat& frame);

    // Handles input coming from an image, used mainly for development purposes
    void HandleImageInput(const std::string& imagePath);

    // Function that returns the model
    std::unique_ptr<tflite::FlatBufferModel>& GetModel();

    // Handles outputting needed data to use in graphical drawing and recognition.
    void HandleOutput(float minimumConfidence);

    void HandleFaceOutput();

    cv::Mat GetInput();

    std::vector<double> GetFaceEmbeddings();

private:
    float* inputTensor;
    int16_t modelParam;
    std::vector<std::string> classLabels;
    std::unique_ptr<tflite::FlatBufferModel> model;
    std::unique_ptr<tflite::Interpreter> interpreter;
    cv::Mat input;
    float* output;
    std::vector<double> faceEmbeddings;

    void DrawBox(int maxConfidenceIndex, float maxConfidence, int numValuesPerDetection);
};