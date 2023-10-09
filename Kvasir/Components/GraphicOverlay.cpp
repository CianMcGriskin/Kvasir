#include "GraphicOverlay.h"

void GraphicOverlay::DrawBox(float* output, int numPerDetect, int i, cv::Mat image) {
    int x = static_cast<int>(output[i * numPerDetect + 0] * image.cols);
    int y = static_cast<int>(output[i * numPerDetect + 1] * image.rows);
    int width = static_cast<int>(output[i * numPerDetect + 2] * image.cols);
    int height = static_cast<int>(output[i * numPerDetect + 3] * image.rows);

// Calculate top-left and bottom-right coordinates of the bounding box
    int x1 = std::max(0, x - width / 2);
    int y1 = std::max(0, y - height / 2);
    int x2 = std::min(image.cols - 1, x + width / 2);
    int y2 = std::min(image.rows - 1, y + height / 2);

    cv::rectangle(image, cv::Rect(x1, y1, x2 - x1, y2 - y1), cv::Scalar(0, 255, 0), 2);

}