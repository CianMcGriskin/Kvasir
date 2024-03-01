#include "StreamService.h"

// Define JPEG quality parameter for image encoding
std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 90};
nadjieb::MJPEGStreamer streamer;

void StreamService::StartStream() {
    streamer.start(8090);
}
// Encodes and sends a video frame to the stream
void StreamService::SendFrame(const cv::Mat& frame) {
    std::vector<uchar> buff_bgr;
    cv::imencode(".jpg", frame, buff_bgr, params);

    // bgr endpoint stream
    streamer.publish("/bgr", std::string(buff_bgr.begin(), buff_bgr.end()));
}

// Stops the video streaming service
void StreamService::StopStream() {
    streamer.stop();
}
