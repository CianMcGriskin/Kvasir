#include "Model.h"

// Constructor of the Model class, passing the model size for image in this case [1, 160, 160, 3]
Model::Model(int16_t modelSize, const std::shared_ptr<NotificationQueue>& notificationQueue)
      :notificationQueue(notificationQueue)
{
    modelParam = modelSize;
    input = cv::Mat(modelSize, modelSize, CV_32FC3);
}

// Function to load a TensorFlow Lite model from a file
void Model::LoadModel(const char* modelPath) {
    model = tflite::FlatBufferModel::BuildFromFile(modelPath);
}

// Fucntion to build TFLite interpreter
void Model::BuildInterpreter() {
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder(*model, resolver)(&interpreter);

    // Resize input tensors to model input tensor size
    interpreter->AllocateTensors();

    // Executing inference on total number of threads available - 2, this was best during testing
    auto threadCount = std::thread::hardware_concurrency();
    interpreter->SetNumThreads((threadCount-2));
}

// Function to handle
void Model::HandleImageInput(const std::string& imagePath) { // Avg 80ms execution all the time
    // Input cropped images
    input = cv::imread(imagePath);

    // Resize image input to size expected from the AI Model
    cv::resize(input, input, cv::Size(modelParam, modelParam));
    input.convertTo(input, CV_32FC3);

    // Normalise input
    input /= 255.0;

    // Copy preprocessed image data to the input tensor
    auto* inputImageTensor = interpreter->typed_input_tensor<float>(0);
    std::memcpy(inputImageTensor, input.data, modelParam * modelParam * 3 * sizeof(float));

    interpreter->Invoke();
}

void Model::HandleFaceOutput() { // avg 2ms execution
    faceEmbeddings.clear();

    FaceStorage faceStorage;

    // Vector to hold faces gathered from JSON file
    std::vector<float> faceData;
    faceData.reserve(512);

    // Number of dimensions in the embedding vector
    const size_t embeddingSize = 512;

    // Get the output tensor from the interpreter
    output = interpreter->typed_output_tensor<float>(0);
    const float* embeddingData = output;

    // Reserve vector size to embedding size of 512
    faceEmbeddings.reserve(embeddingSize);

    // For each element of face embedding vector, add the result of the output i.e. the face data
    for (size_t i = 0; i < embeddingSize; ++i)
    {
        faceEmbeddings.emplace_back(embeddingData[i]);
    }

    // Get JSON data of the face storage database
    auto jsonData = faceStorage.GetJsonData();

    // Get number of people within the JSON file
    size_t numberOfPeople = jsonData.size();

    // Loop over each person index
    for (size_t personIndex = 0; personIndex < numberOfPeople; ++personIndex)
    {
        // Convert index to string for JSON key
        std::string key = std::to_string(personIndex);

        // If jsonData has the person index and faces is not empty then we need to compare those face vectors
        if (jsonData.contains(key) && !jsonData[key]["faces"].empty())
        {
            // Detects how many face vectors we have of that specific person
            size_t facesCount = jsonData[key]["faces"].size();

            // Loop over every face array of that particular person
            for (size_t faceIndex = 0; faceIndex < facesCount; ++faceIndex)
            {
                faceData.clear();

                // Get current face vector
                faceData = faceStorage.RetrieveFace(personIndex, faceIndex);

                // Get similarity between current face and saved face within the JSON
                float similarity = FaceDetection::CompareFaces(faceEmbeddings, faceData);

                // If similarity is greater than threshold then we are confident that it is that person
                if (similarity > 0.85)
                {
                    // Print detection
                    std::cout << "Detected: " << jsonData[key]["Name"] << " with a similarity of " << similarity << std::endl;

                    // Create a notification to push to the queue
                    NotificationQueue::Notification notification;
                    notification.name = jsonData[key]["Name"];
                    notification.reason = jsonData[key]["Reason"];
                    notification.confidence = similarity;

                    // Add the prepared notification to the notification queue
                    notificationQueue->push(notification);
                }
            }
        }
    }
}

std::vector<float_t> Model::ProcessImage(const std::string& imagePath)
{
    std::vector<float_t> imageFaceEmbeddings;

    input = cv::imread(imagePath);
    cv::resize(input, input, cv::Size(modelParam, modelParam));
    input.convertTo(input, CV_32FC3);
    input /= 255.0;

    auto* inputImageTensor = interpreter->typed_input_tensor<float>(0);
    std::memcpy(inputImageTensor, input.data, modelParam * modelParam * 3 * sizeof(float));

    interpreter->Invoke();

    const float* embeddingData = interpreter->typed_output_tensor<float>(0);

    const size_t embeddingSize = 512;
    imageFaceEmbeddings.reserve(embeddingSize);

    for (size_t i = 0; i < embeddingSize; ++i)
    {
        imageFaceEmbeddings.emplace_back(embeddingData[i]);
    }

    return imageFaceEmbeddings;
}

// Returns TFLite Model
std::unique_ptr<tflite::FlatBufferModel>& Model::GetModel() {
    return model;
}

// Get input of model
cv::Mat Model::GetInput() {
    return input;
}
