#include "./Components/CameraControl.h"
#include "./Components/CameraConnection.h"
#include "./Components/Model.h"


int main() {
    // Load the model using model class
    Model modelInstance;
    CameraControl camera;

    camera.TakePicture("/home/cian/dev/Kvasir/Kvasir/capture.jpg");
    modelInstance.LoadModel("/home/cian/dev/Kvasir/Kvasir/yolov5.tflite");

    modelInstance.BuildInterpreter();
    modelInstance.HandleInput(320, "/home/cian/dev/Kvasir/Kvasir/capture.jpg");
    //modelInstance.HandleOutput();

    int num_detections = 6300;
    int num_values_per_detection = 85;
    float min_confidence_threshold = 0.3;
    float* output = modelInstance.GetInterpreter()->typed_output_tensor<float>(0);

    // Visualize detected regions with confidence above the threshold
    for (int i = 0; i < num_detections; ++i) {
        float class_score = output[i * num_values_per_detection + 4];  // Assuming the class score is at index 4

        // Check if the class score is above the threshold
        if (class_score > min_confidence_threshold) {
            int class_index = static_cast<int>(output[i * num_values_per_detection]);  // Assuming class index is at the beginning
            std::string class_name = class_labels[class_index];  // Get class name based on class index

            // Print class name and confidence score
            std::cout << "Detected: " << class_name << " (confidence: " << class_score << ")" << std::endl;

            // Visualize the detected region by drawing a bounding box
            int x = static_cast<int>(output[i * num_values_per_detection + 0] * image.cols);  // x-coordinate
            int y = static_cast<int>(output[i * num_values_per_detection + 1] * image.rows);  // y-coordinate
            int width = static_cast<int>(output[i * num_values_per_detection + 2] * image.cols);  // width
            int height = static_cast<int>(output[i * num_values_per_detection + 3] * image.rows);  // height

            // Draw rectangle around the detected region
            cv::rectangle(image, cv::Rect(x, y, width, height), cv::Scalar(0, 255, 0), 2);
        }
    }

    // Display the image with detection results
    cv::imshow("Detection Results", image);
    cv::waitKey(0);  // Wait for a key event

    return 0;

    cv::imshow("Detection Results", modelInstance.GetInput());
    cv::waitKey(0);  // Wait for a key event

    return 0;

}
