#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include "Model.h"

// Function to load a TensorFlow Lite model from a file
void Model::LoadModel(const char* modelPath) {
    model = tflite::FlatBufferModel::BuildFromFile(modelPath);
    if (!model)
    {
        loaded = false;
    }
    else
    {
        loaded = true;
    }
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
}

void Model::HandleInput(int16_t modelSize, std::string imagePath) {
    // [1, 320, 320, 3]
    cv::Mat image = cv::imread(imagePath);
    cv::resize(image, image, cv::Size(modelSize, modelSize));
    image.convertTo(image, CV_32FC4);
    image /= 255.0;

    // Copy preprocessed image data to the input tensor
    float* input = interpreter->typed_input_tensor<float>(0);
    std::memcpy(input, image.data, modelSize * modelSize * 3 * sizeof(float));

    interpreter->Invoke();
}

void Model::HandleOutput(){
    float* output = interpreter->typed_output_tensor<float>(0);
    std::vector<std::string> labels = Model::LoadIdentityLabels("/home/cian/dev/Kvasir/Kvasir/labels.txt");
    for (int i = 0; i < 85; ++i) {
            int class_index = static_cast<int>(output[i * 85]);  // Assuming class index is at the beginning
            std::string class_name = labels[class_index];

            std::cout << "Detected: " << class_name << " (confidence: " << class_index << ")" << std::endl;
    }
}

bool Model::IsModelLoaded() const {
    return loaded;
}

std::unique_ptr<tflite::FlatBufferModel>& Model::GetModel() {
    return model;
}