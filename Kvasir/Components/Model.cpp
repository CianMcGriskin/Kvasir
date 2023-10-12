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
    input.convertTo(input, CV_32FC3);
    input /= 255.0;

    // Copy preprocessed image data to the input tensor
    float* inputTensor = interpreter->typed_input_tensor<float>(0);
    std::memcpy(inputTensor, input.data, modelSize * modelSize * 3 * sizeof(float));

    interpreter->Invoke();
}

void Model::HandleOutput(float minimumConfidence){
    output = interpreter->typed_output_tensor<float>(0);
    std::vector<std::string> labels = Model::LoadIdentityLabels("/home/cian/dev/Kvasir/Kvasir/labels.txt");
    int numDetections = 100;
    const int numValuesPerDetection = labels.size();

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
            int x2 = x1 + width;
            int y2 = y1 + height;

            std::cout << "Detected person" << " (confidence: " << class_score << ")" << std::endl;

            // Draw rectangle around the detected region
            cv::rectangle(input, cv::Rect(x1, y1, width, height), cv::Scalar(0, 255, 0), 2);
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