#ifndef FACE_DETECTION_H
#define FACE_DETECTION_H

#include "FaceDetection.h"

FaceDetection::FaceDetection() {
    std::string protoPath = "../../Kvasir/Components/Models/FaceDetectionCaffeModel/deploy.prototxt";
    std::string caffeModelPath = "../../Kvasir/Components/Models/FaceDetectionCaffeModel/res10_300x300_ssd_iter_140000_fp16.caffemodel";
    faceDetectionModel = cv::dnn::readNetFromCaffe(protoPath, caffeModelPath);
}

void FaceDetection::DetectFaces(cv::Mat &image, float confidenceLevel, bool display) {
    int imageHeight = image.rows;
    int imageWidth = image.cols;
    
    // Mean and size for the model params
    cv::Size size = cv::Size(300, 300);
    cv::Scalar blobMean = cv::Scalar(104.0, 117.0, 123.0);

    // Clone image to process it
    cv::Mat outputImage = image.clone();

    // Process image
    cv::Mat preprocessedImage;
    cv::dnn::blobFromImage(image, preprocessedImage, 1.0, size, blobMean, false, false);

    // Set image as input
    faceDetectionModel.setInput(preprocessedImage);

    cv::Mat results = faceDetectionModel.forward();

    cv::Mat detectionMat(results.size[2], results.size[3], CV_32F, results.ptr<float>());

    for (int i = 0; i < detectionMat.rows; i++) {
        float confidence = detectionMat.at<float>(i, 2);

        if (confidence > confidenceLevel) {
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * imageWidth);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * imageHeight);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * imageWidth);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * imageHeight);

            cv::rectangle(outputImage, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), imageWidth / 200);
//            cv::rectangle(outputImage, cv::Point(x1, y1 - imageWidth / 20), cv::Point(x1 + imageWidth / 16, y1), cv::Scalar(0, 255, 0), -1);
//            std::string conf_str = std::to_string(static_cast<int>(confidence * 100)) + "%";
//            cv::putText(outputImage, conf_str, cv::Point(x1, y1 - 25), cv::FONT_HERSHEY_COMPLEX, imageWidth / 700, cv::Scalar(255, 255, 255), imageWidth / 200);
        }
    }

    if (display)
    {
        cv::imshow("Output", outputImage);
        cv::waitKey(0);
    }
}

cv::dnn::Net FaceDetection::GetModel() {
    return cv::dnn::Net();
}

#endif /* FACE_DETECTION_H */