#include "FaceDetection.h"

// Initialisation of variable
unsigned char FaceDetection::numOfFacesDetected = 0;

// Constructor to initalise caffe model
FaceDetection::FaceDetection() { // Avg 10ms once off exectuion time
    std::string protoPath = "../../Kvasir/Components/Models/FaceDetectionCaffeModel/deploy.prototxt";
    std::string caffeModelPath = "../../Kvasir/Components/Models/FaceDetectionCaffeModel/res10_300x300_ssd_iter_140000_fp16.caffemodel";

    // Using the opencv caffe model execution
    faceDetectionModel = cv::dnn::readNetFromCaffe(protoPath, caffeModelPath);
}

// Function used to detect faces within an image
void FaceDetection::DetectFaces(cv::Mat &image, float confidenceLevel, bool display) { // 70ms avg execution time
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

    // Initialise a cv::Mat object to hold the detection results
    cv::Mat detectionMat(results.size[2], results.size[3], CV_32F, results.ptr<float>());

    // For every face detection
    for (int i = 0; i < detectionMat.rows; ++i)
    {
        // Get confidence of faces detected within image
        float confidence = detectionMat.at<float>(i, 2);

        // Checking for confidence level set at 0.5
        if (confidence > confidenceLevel)
        {
            // Calculate the bounding box for the detected face.
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * imageWidth);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * imageHeight);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * imageWidth);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * imageHeight);

            // Rectangular region around the detected face
            cv::Rect faceRegion(x1, y1, x2 - x1, y2 - y1);
            try
            {
                // Crop the detected face from the image
                cv::Mat croppedFace = image(faceRegion).clone();

                // Store the cropped face
                croppedFaces.push_back(croppedFace);
            }
            catch (cv::Exception &e)
            {
                // Empty as it prints out an error if the face bounding box goes slightly out of frame, error doesn't affect application
            }

            // Draw a rectangle around the detected face on the output image
            cv::rectangle(outputImage, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), imageWidth / 200);
        }
    }

    // Set number of faces detected within the image
    numOfFacesDetected = croppedFaces.size();

    // If display is set to true then display the faces being captured
    if (display)
    {
        cv::imshow("Output", outputImage);
        cv::waitKey(1);
    }


    std::string outputDirectory = "../../Kvasir/Components/Output";

    // For all faces detected within the image, save the face to a cropped image
    for (size_t i = 0; i < croppedFaces.size(); ++i)
    {
        std::string filename = outputDirectory + "/cropped_face_" + std::to_string(i + 1) + ".jpg";
        cv::imwrite(filename, croppedFaces[i]);
        cv::imshow("Cropped Face " + std::to_string(i + 1), croppedFaces[i]);
        cv::waitKey(1);
    }
}

bool FaceDetection::DetectFaceWithinImage(cv::Mat &image, float confidenceLevel) {
    std::string protoPath = "../../Kvasir/Components/Models/FaceDetectionCaffeModel/deploy.prototxt";
    std::string caffeModelPath = "../../Kvasir/Components/Models/FaceDetectionCaffeModel/res10_300x300_ssd_iter_140000_fp16.caffemodel";
    auto tempFaceDetectionModel = cv::dnn::readNetFromCaffe(protoPath, caffeModelPath);
    
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
    tempFaceDetectionModel.setInput(preprocessedImage);
    cv::Mat results = tempFaceDetectionModel.forward();

    // Initialise a cv::Mat object to hold the detection results
    cv::Mat detectionMat(results.size[2], results.size[3], CV_32F, results.ptr<float>());

    // For every face detection
    for (int i = 0; i < detectionMat.rows; ++i)
    {
        // Get confidence of faces detected within image
        float confidence = detectionMat.at<float>(i, 2);

        // Checking for confidence level set at 0.5
        if (confidence > confidenceLevel)
        {
            // Calculate the bounding box for the detected face.
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * imageWidth);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * imageHeight);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * imageWidth);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * imageHeight);

            // Rectangular region around the detected face
            cv::Rect faceRegion(x1, y1, x2 - x1, y2 - y1);
            try
            {
                // Crop the detected face from the image
                cv::Mat croppedFace = image(faceRegion).clone();

                // Store the cropped face
                croppedFaces.push_back(croppedFace);
            }
            catch (cv::Exception &e)
            {
                // Empty as it prints out an error if the face bounding box goes slightly out of frame, error doesn't affect application
            }

            // Draw a rectangle around the detected face on the output image
            cv::rectangle(outputImage, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), imageWidth / 200);
        }
    }

    unsigned long numOfFaces = croppedFaces.size();

    std::string outputDirectory = "../../Kvasir/TempPersonImage";
    if (numOfFaces > 1 || numOfFaces == 0)
    {
        return false;
        // Can't be more than one face in an image - delete it
    }
    else
    {
        // If one face is detected, save it to the specified directory and return true.
        std::string filename = outputDirectory + "/cropped_face_" + std::to_string(numOfFaces) + ".jpg";
        cv::imwrite(filename, croppedFaces[0]);
        return true;
    }
}

float FaceDetection::CompareFaces(std::vector<float> currentFace, std::vector<float> savedFace, short size) { // 0 ms
    // Initialising dot product and norm
    float dotProduct = 0.0;
    float normA = 0.0;
    float normB = 0.0;

    // Check if the vectors have 512 elements for comparison
    if (currentFace.size() < size || savedFace.size() < size)
    {
        // If not error is printed
        std::cerr << "\nFace vectors do not not meet sufficient elements for comparison.\n";
        std::cerr << "Current Face Captured Size: " << currentFace.size() << "\n";
        std::cerr << "Saved Face Captured Size: " << savedFace.size() << "\n";
        return 0.0;
    }

    // Iterate over the elements of the vectors
    for (size_t i = 0; i < size; ++i)
    {
        // Calculate dot product of the two vectors
        dotProduct += savedFace[i] * currentFace[i];

        // Calculate the square of the norm of the first vector
        normA += savedFace[i] * savedFace[i];

        // Calculate the square of the norm of the second vector
        normB += currentFace[i] * currentFace[i];
    }

    // Check if either norm is zero to prevent division by zero
    if (normA == 0 || normB == 0)
    {
        return 0.0;
    }

    // Calculate and return the cosine similarity score
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

size_t FaceDetection::GetNumOfFacesDetected() {
    return numOfFacesDetected;
}

cv::dnn::Net FaceDetection::GetModel() {
    return faceDetectionModel;
}