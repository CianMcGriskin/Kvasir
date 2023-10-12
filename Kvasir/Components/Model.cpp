#include "Model.h"
#include "GraphicOverlay.h"

#include <chrono>

Model::Model(int16_t modelSize) {
    input = cv::Mat(modelSize, modelSize, CV_32FC3);
    std::vector<std::string> labels = Model::LoadIdentityLabels("../../Kvasir/labels.txt");
}

// Function to load a TensorFlow Lite model from a file
void Model::LoadModel(const char* modelPath) {
    model = tflite::FlatBufferModel::BuildFromFile(modelPath);
}

std::vector<std::string> Model::LoadIdentityLabels(const std::string& labelsPath) {
    std::vector<std::string> labels;
    std::ifstream file(labelsPath);

    if (file.is_open())
    {
        std::string label;
        while (std::getline(file, label))
        {
            labels.emplace_back(label);
        }
        file.close();
    }
    else
    {
        std::cerr << "Failed to open class labels file: " << labelsPath << std::endl;
    }
    return labels;
}

void Model::BuildInterpreter() {
    // Build interpreter
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder(*model, resolver)(&interpreter);

    // Resize input tensors to model input tensor size
    interpreter->AllocateTensors();
    inputTensor = interpreter->typed_input_tensor<float>(0);
}

//void Model::HandleInput(int16_t modelSize, std::string imagePath) {
//    // [1, 320, 320, 3]
//    input = cv::imread(imagePath);
//    cv::resize(input, input, cv::Size(modelSize, modelSize));
//    input.convertTo(input, CV_32FC3);
//    input /= 255.0;
//
//    // Copy preprocessed image data to the input tensor
//    float* inputTensor = interpreter->typed_input_tensor<float>(0);
//    std::memcpy(inputTensor, input.data, modelSize * modelSize * 3 * sizeof(float));
//
//    interpreter->Invoke();
//}

void Model::HandleInput(int16_t modelSize, const cv::Mat& frame) {
    // Start measuring the execution time
    auto start_time = std::chrono::high_resolution_clock::now();

    // Resize the input frame to match the model input size
    cv::resize(frame, input, cv::Size(modelSize, modelSize));
    input.convertTo(input, CV_32FC3);

    // Copy preprocessed image data to the input tensor
    std::memcpy(inputTensor, input.data, modelSize * modelSize * 3 * sizeof(float));

    // Invoke the interpreter
    interpreter->Invoke();

    // Stop measuring the execution time
    auto end_time = std::chrono::high_resolution_clock::now();

    // Calculate and print the execution time in milliseconds
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "HandleInput execution time: " << duration.count() << " milliseconds" << std::endl;
}


void Model::HandleOutput(float minimumConfidence){
    // Values taken from output tensor
    const int numValuesPerDetection = 85;
    const int numDetections = 6300;

    output = interpreter->typed_output_tensor<float>(0);

    for (int i = 0; i < numDetections; ++i) {
        float class_score = output[i * numValuesPerDetection + 4];
        
        // Check if the class score is above the threshold
        if (class_score > minimumConfidence) {
            int x_center = static_cast<int>(output[i * numValuesPerDetection + 0] * input.cols);  // x_center
            int y_center = static_cast<int>(output[i * numValuesPerDetection + 1] * input.rows);  // y_center
            int width = static_cast<int>(output[i * numValuesPerDetection + 2] * input.cols);  // width
            int height = static_cast<int>(output[i * numValuesPerDetection + 3] * input.rows);  // height

            // Calculate top-left and bottom-right coordinates of the bounding box
            int x1 = x_center - width / 2;
            int y1 = y_center - height / 2;

            std::cout << class_score << "Conf";

            // Draw rectangle around the detected region
            cv::rectangle(input, cv::Rect(x1, y1, width, height), cv::Scalar(0, 255, 0), 2);
        }
    }
}

std::unique_ptr<tflite::FlatBufferModel>& Model::GetModel() {
    return model;
}

cv::Mat Model::GetInput() {
    return input;
}