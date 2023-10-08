#include <iostream>
#include <fstream>
#include <vector>
#include "./Components/CameraControl.h"
#include "./Components/CameraConnection.h"
#include "./Components/Model.h"

// Function to load class labels from a file
std::vector<std::string> loadClassLabels(const std::string& labels_file) {
    std::vector<std::string> class_labels;
    std::ifstream file(labels_file);
    if (file.is_open()) {
        std::string label;
        while (std::getline(file, label)) {
            class_labels.push_back(label);
        }
        file.close();
    } else {
        std::cerr << "Failed to open class labels file: " << labels_file << std::endl;
    }
    return class_labels;
}

int main() {
    // Load the model using model class
    Model modelInstance;
    CameraControl camera;

    camera.TakePicture("/home/cian/dev/Kvasir/Kvasir/capture.jpg");
    modelInstance.LoadModel("/home/cian/dev/Kvasir/Kvasir/yolov5.tflite");
    std::cout << (modelInstance.IsModelLoaded());

    modelInstance.BuildInterpreter();
    modelInstance.HandleInput(320, "/home/cian/dev/Kvasir/Kvasir/capture.jpg");
    modelInstance.HandleOutput();
}
