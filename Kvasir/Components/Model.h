#include <tensorflow/lite/model.h>
#include "tensorflow/lite/core/kernels/register.h"
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

class Model {
public:
    Model(int16_t modelSize);

// Function used to load model from File
    void LoadModel(const char* modelPath);

    // Function used to load label identifiers
    std::vector<std::string> LoadIdentityLabels(const std::string& labelsFilePath);

    // Function to build interpreter - driver class to run pre-trained models
    void BuildInterpreter();

    // Handles the input, conversions to fit model criteria
    void HandleInput(int16_t modelSize, const cv::Mat& frame);

    // Function that returns the model
    std::unique_ptr<tflite::FlatBufferModel>& GetModel();

    // Handles outputting needed data to use in graphical drawing and recognition.
    void HandleOutput(float minimumConfidence);

    cv::Mat GetInput();

private:
    float* inputTensor;
    std::unique_ptr<tflite::FlatBufferModel> model;
    std::unique_ptr<tflite::Interpreter> interpreter;
    cv::Mat input;
    float* output;
};