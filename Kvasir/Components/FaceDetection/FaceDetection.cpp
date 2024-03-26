#include "FaceDetection.h"

// Initialisation of variable
unsigned char FaceDetection::numOfFacesDetected = 0;

// Constructor to initalise caffe model
FaceDetection::FaceDetection() { // Avg 10ms once off exectuion time
    std::string protoPath = "../../Kvasir/Components/Models/FaceDetectionCaffeModel/deploy.prototxt";
    std::string caffeModelPath = "../../Kvasir/Components/Models/FaceDetectionCaffeModel/res10_300x300_ssd_iter_140000_fp16.caffemodel";
    faceDetectionModel = cv::dnn::readNetFromCaffe(protoPath, caffeModelPath);
}

// Function used to detect faces within an image
void FaceDetection::DetectFaces(cv::Mat &image, float confidenceLevel, bool display) { // 70ms avg execution time constant
    int imageHeight = image.rows;
    int imageWidth = image.cols;

    // Mean and size for the model params
    cv::Size size = cv::Size(300, 300);
    cv::Scalar blobMean = cv::Scalar(104.0, 117.0, 123.0);

    // Clone image to process it
    cv::Mat outputImage = image.clone();

    // Save face images in a vector
    std::vector<cv::Mat> croppedFaces;

    // Process image
    cv::Mat preprocessedImage;
    cv::dnn::blobFromImage(image, preprocessedImage, 1.0, size, blobMean, false, false);

    // Set image as input
    faceDetectionModel.setInput(preprocessedImage);
    cv::Mat results = faceDetectionModel.forward();


    cv::Mat detectionMat(results.size[2], results.size[3], CV_32F, results.ptr<float>());

    for (int i = 0; i < detectionMat.rows; ++i)
    {
        float confidence = detectionMat.at<float>(i, 2);

        if (confidence > confidenceLevel)
        {
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * imageWidth);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * imageHeight);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * imageWidth);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * imageHeight);

            if (x1 >= 0 && y1 >= 0 && x2 >= 0 && y2 >= 0 && x2 >= x1 && y2 >= y1 && x2 <= imageWidth && y2 <= imageHeight)
            {
                cv::Rect faceRegion(x1, y1, x2 - x1, y2 - y1);
                cv::Mat croppedFace = image(faceRegion).clone();
                croppedFaces.push_back(croppedFace);
                cv::rectangle(outputImage, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), imageWidth / 200);
            }
        }
    }

    numOfFacesDetected = croppedFaces.size();

    if (display)
    {
        cv::imshow("Output", outputImage);
        cv::waitKey(1);
    }

    std::string outputDirectory = "../../Kvasir/Components/Output";
    for (size_t i = 0; i < croppedFaces.size(); ++i) {
        std::string filename = outputDirectory + "/cropped_face_" + std::to_string(i + 1) + ".jpg";
        cv::imwrite(filename, croppedFaces[i]);
        cv::imshow("Cropped Face " + std::to_string(i + 1), croppedFaces[i]);
        cv::waitKey(1);
    }
}

bool FaceDetection::DetectFaceWithinImage(cv::Mat &image, float confidenceLevel) {
    int imageHeight = image.rows;
    int imageWidth = image.cols;

    // Mean and size for the model params
    cv::Size size = cv::Size(300, 300);
    cv::Scalar blobMean = cv::Scalar(104.0, 117.0, 123.0);

    // Clone image to process it
    cv::Mat outputImage = image.clone();

    // Save face images in a vector
    std::vector<cv::Mat> croppedFaces;

    // Process image
    cv::Mat preprocessedImage;
    cv::dnn::blobFromImage(image, preprocessedImage, 1.0, size, blobMean, false, false);

    // Set image as input
    faceDetectionModel.setInput(preprocessedImage);
    cv::Mat results = faceDetectionModel.forward();


    cv::Mat detectionMat(results.size[2], results.size[3], CV_32F, results.ptr<float>());

    for (int i = 0; i < detectionMat.rows; ++i) {
        float confidence = detectionMat.at<float>(i, 2);

        if (confidence > confidenceLevel) {
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * imageWidth);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * imageHeight);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * imageWidth);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * imageHeight);

            cv::Rect faceRegion(x1, y1, x2 - x1, y2 - y1);
            try {
                cv::Mat croppedFace = image(faceRegion).clone();
                croppedFaces.push_back(croppedFace);
            }
            catch (cv::Exception &e) {
                // Empty as it prints out an error if the face bounding box goes slightly out of frame, error doesn't affect application
            }

            cv::rectangle(outputImage, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), imageWidth / 200);
        }
    }

    unsigned long numOfFaces = croppedFaces.size();

    std::string outputDirectory = "../../Kvasir/TempPersonImage";
    if (numOfFaces > 1 || numOfFaces == 0)
    {
        return false;
        // Can't be more than one face in an image - delete it
    } else {
        std::string filename = outputDirectory + "/cropped_face_" + std::to_string(numOfFaces) + ".jpg";
        cv::imwrite(filename, croppedFaces[0]);
        return true;
    }

}

float FaceDetection::CompareFaces(std::vector<float> currentFace, std::vector<float> savedFace, short size) { // 0 ms
    float dotProduct = 0.0;
    float normA = 0.0;
    float normB = 0.0;

    if (currentFace.size() < size || savedFace.size() < size) {
        std::cerr << "\nFace vectors do not not meet sufficient elements for comparison.\n";
        std::cerr << "Current Face Captured Size: " << currentFace.size() << "\n";
        std::cerr << "Saved Face Captured Size: " << savedFace.size() << "\n";
        return 0.0;
    }

    for (size_t i = 0; i < size; ++i)
    {
        dotProduct += savedFace[i] * currentFace[i];
        normA += savedFace[i] * savedFace[i];
        normB += currentFace[i] * currentFace[i];
    }

    if (normA == 0 || normB == 0)
    {
        return 0.0;
    }

    //std::cout << "\nSimilarity: " << dotProduct / (std::sqrt(normA) * std::sqrt(normB));
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

size_t FaceDetection::GetNumOfFacesDetected() {
    return numOfFacesDetected;
}

cv::dnn::Net FaceDetection::GetModel() {
    return faceDetectionModel;
}