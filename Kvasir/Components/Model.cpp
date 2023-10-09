#include "Model.h"
#include "GraphicOverlay.h"

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
    input = cv::imread(imagePath);
    cv::resize(input, input, cv::Size(modelSize, modelSize));
    input.convertTo(input, CV_32FC4);
    input /= 255.0;

    // Copy preprocessed image data to the input tensor
    float* inputTensor = interpreter->typed_input_tensor<float>(0);
    std::memcpy(inputTensor, input.data, modelSize * modelSize * 3 * sizeof(float));

    interpreter->Invoke();
}

void Model::HandleOutput(){
    output = interpreter->typed_output_tensor<float>(0);
    std::vector<std::string> labels = Model::LoadIdentityLabels("/home/cian/dev/Kvasir/Kvasir/labels.txt");
    int numDetections = 6300;
    const int numValuesPerDetection = labels.size();
    float min_confidence_threshold = 0.3;


    for (int i = 0; i < numDetections; ++i)
    {
        float confidence = output[i * numValuesPerDetection + 4];

        if(confidence > min_confidence_threshold)
        {
            int classIndex = static_cast<int>(output[i * numValuesPerDetection]);
            std::string class_name = labels[classIndex];
            GraphicOverlay::DrawBox(output, numValuesPerDetection, i, input);
            std::cout << "Detected: " << class_name << " (confidence: " << confidence << ")" << std::endl;
        }
    }
}

bool Model::IsModelLoaded() const {
    return loaded;
}

std::unique_ptr<tflite::FlatBufferModel>& Model::GetModel() {
    return model;
}

cv::Mat Model::GetInput() {
    return input;
}

std::unique_ptr<tflite::Interpreter>& Model::GetInterpreter() {
    return interpreter;
}