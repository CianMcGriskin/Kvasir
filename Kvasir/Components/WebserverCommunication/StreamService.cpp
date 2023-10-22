#include "StreamService.h"

std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 90};
nadjieb::MJPEGStreamer streamer;

void StreamService::StartStream() {
    streamer.start(8080);

    std::vector<uchar> buff_bgr;
    streamer.publish("/bgr", std::string(buff_bgr.begin(), buff_bgr.end()));
}

void StreamService::StopStream() {
    streamer.stop();
}
