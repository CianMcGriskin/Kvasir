#include "Model.h"

FaceStorage faceStorage;

// Constructor of the Model class, passing the model size for image in this case [1, 320, 320, 3]
Model::Model(int16_t modelSize) {
    modelParam = modelSize;
    input = cv::Mat(modelSize, modelSize, CV_32FC3);
    // classLabels = Model::LoadIdentityLabels("../../Kvasir/labels.txt");
}

// Function to load a TensorFlow Lite model from a file
void Model::LoadModel(const char* modelPath) {
    model = tflite::FlatBufferModel::BuildFromFile(modelPath);
}

// Function used to load text labels, used to display what the model is detecting
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
    interpreter->SetNumThreads(6);
}

void Model::HandleImageInput(const std::string& imagePath) {
    // Used to calculate performance time
    auto startTime = std::chrono::high_resolution_clock::now();
    input = cv::imread(imagePath);
    cv::resize(input, input, cv::Size(modelParam, modelParam));
    input.convertTo(input, CV_32FC3);
    input /= 255.0;

    // Copy preprocessed image data to the input tensor
    auto* inputImageTensor = interpreter->typed_input_tensor<float>(0);
    std::memcpy(inputImageTensor, input.data, modelParam * modelParam * 3 * sizeof(float));

    interpreter->Invoke();

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "HandleInput execution time: " << duration.count() << " milliseconds" << std::endl;
}

void Model::HandleInput(int16_t modelSize, const cv::Mat& frame) {
    // Start measuring the execution time
    auto start_time = std::chrono::high_resolution_clock::now();

    // Resize the input frame to match the model input size
    cv::resize(frame, input, cv::Size(modelSize, modelSize));
    input.convertTo(input, CV_32FC3);
    input /= 255.0;

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

void Model::HandleFaceOutput() {
    // Number of dimensions in the embedding vector
    const size_t embeddingSize = 512;

    // Get the output tensor from the interpreter
    output = interpreter->typed_output_tensor<float>(0);

    const float* embeddingData = output;

    // Print the embeddings to the console
    std::vector<float> embedding;
    faceEmbeddings.reserve(embeddingSize);

    std::cout << "Face Embeddings: ";
    for (size_t i = 0; i < embeddingSize; ++i)
    {
        faceEmbeddings.emplace_back(embeddingData[i]);
        std::cout << embeddingData[i] << ", ";
    }

    size_t sizeInBytes = sizeof(faceEmbeddings);
    std::cout << sizeInBytes;

    if(!faceStorage.GetJsonData().contains("faces") || !faceStorage.GetJsonData()["faces"].empty())
    {
        for(size_t i = 0; i <= faceStorage.GetJsonData().size(); ++i)
        {
            FaceDetection::CompareFaces(faceEmbeddings, faceStorage.RetrieveFace(i));
        }
    }

    std::cout << std::endl;
}

void Model::HandleOutput(float minimumConfidence) {
    // Values taken from output tensor
    const unsigned char numValuesPerDetection = 85;
    const size_t numDetections = 6300;

    output = interpreter->typed_output_tensor<float>(0);

    int maxConfidenceIndex = -1;
    float maxConfidence = minimumConfidence;

    // Find the index with the maximum confidence score above the threshold
    for (uint16_t i = 0; i < numDetections; ++i)
    {
        float class_score = output[i * numValuesPerDetection + 4];

        if (class_score > minimumConfidence && class_score > maxConfidence)
        {
            maxConfidence = class_score;
            maxConfidenceIndex = i;
        }
    }

    // If an object above the threshold is found, draw a rectangle around it
    if (maxConfidenceIndex != -1)
    {
        DrawBox(maxConfidenceIndex, maxConfidence, numValuesPerDetection);
    }
}

void Model::DrawBox(int maxConfidenceIndex, float maxConfidence, int numValuesPerDetection){
    size_t x_center = static_cast<int>(output[maxConfidenceIndex * numValuesPerDetection] * input.cols);  // x_center
    size_t y_center = static_cast<int>(output[maxConfidenceIndex * numValuesPerDetection + 1] * input.rows);  // y_center
    size_t width = static_cast<int>(output[maxConfidenceIndex * numValuesPerDetection + 2] * input.cols);  // width
    size_t height = static_cast<int>(output[maxConfidenceIndex * numValuesPerDetection + 3] * input.rows);  // height

    // Calculate top-left and bottom-right coordinates of the bounding box around the face
    size_t faceHeight = height * 0.33;  // Adjusting the height of the bounding box to focus on the face
    size_t faceWidth = width * 0.40;

    // Calculate top-left and bottom-right coordinates of the bounding box around the face
    size_t faceY = y_center - faceHeight / 3;
    size_t faceX = x_center - width / 2;

    faceX = faceX + 30;
    faceY = faceY - 55;


    std::string classLabel = classLabels[static_cast<int>(output[maxConfidenceIndex * numValuesPerDetection + 4])];

    // Draw rectangle around the detected face region
    cv::rectangle(input, cv::Rect(faceX, faceY, faceWidth, faceHeight), cv::Scalar(0, 255, 0), 2);
    std::string label = classLabel + ": " + std::to_string(maxConfidence);

    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    float fontScale = 0.5;
    unsigned char thickness = 1;
    cv::Size textSize = cv::getTextSize(label, fontFace, fontScale, thickness, nullptr);
    int textX = faceX + (faceWidth - textSize.width) / 2;
    int textY = faceY - 10;
    cv::putText(input, label, cv::Point(textX, textY), fontFace, fontScale, cv::Scalar(0, 255, 0), thickness);
    std::cout << "Face X: " << faceX << ", Face Y: " << faceY << ", Face Width: " << faceWidth << ", Face Height: " << faceHeight << std::endl;
}


std::unique_ptr<tflite::FlatBufferModel>& Model::GetModel() {
    return model;
}

cv::Mat Model::GetInput() {
    return input;
}

std::vector<float> Model::GetFaceEmbeddings() {
    return faceEmbeddings;
}
