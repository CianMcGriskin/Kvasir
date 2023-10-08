#include <tensorflow/lite/model.h>
#include "tensorflow/lite/core/kernels/register.h"
#include <iostream>
#include <fstream>

class Model {
public:
    // Function used to load model from File
    void LoadModel(const char* modelPath);

    // Function used to load label identifiers
    std::vector<std::string> LoadIdentityLabels(const std::string& labelsFilePath);

    void BuildInterpreter();

    void HandleInput(int16_t modelSize, std::string imagePath);

    // Function to check if model has been loaded or not
    bool IsModelLoaded() const;

    // Function that returns the model
    std::unique_ptr<tflite::FlatBufferModel>& GetModel();

    void HandleOutput();

private:
    std::unique_ptr<tflite::FlatBufferModel> model;
    std::unique_ptr<tflite::Interpreter> interpreter;
    bool loaded;
};