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
    //modelInstance.HandleOutput(0.1);

    int num_detections = 6300;
    int num_values_per_detection = 85;
    float min_confidence_threshold = 0.5;
    float* output = modelInstance.GetInterpreter()->typed_output_tensor<float>(0);
    std::vector<std::string> class_labels = modelInstance.LoadIdentityLabels("/home/cian/dev/Kvasir/Kvasir/labels.txt");
    cv::Mat inputVar = modelInstance.GetInput();

    // Visualize detected regions with confidence above the threshold
    for (int i = 0; i < num_detections; ++i) {
        float class_score = output[i * num_values_per_detection + 4];  // Assuming the class score is at index 4
        // Check if the class score is above the threshold
        if (class_score > min_confidence_threshold) {
            int x_center = static_cast<int>(output[i * num_values_per_detection + 0] * inputVar.cols);  // x_center
            int y_center = static_cast<int>(output[i * num_values_per_detection + 1] * inputVar.rows);  // y_center
            int width = static_cast<int>(output[i * num_values_per_detection + 2] * inputVar.cols);  // width
            int height = static_cast<int>(output[i * num_values_per_detection + 3] * inputVar.rows);  // height

            // Calculate top-left and bottom-right coordinates of the bounding box
            int x1 = x_center - width / 2;
            int y1 = y_center - height / 2;
            int x2 = x1 + width;
            int y2 = y1 + height;

            std::cout << "Detected person" << " (confidence: " << class_score << ")" << std::endl;

            // Draw rectangle around the detected region
            cv::rectangle(inputVar, cv::Rect(x1, y1, width, height), cv::Scalar(0, 255, 0), 2);
        }
    }

    // Display the image with detection results
    cv::imshow("Detection Results", inputVar);
    cv::waitKey(0);  // Wait for a key event

    return 0;
}
