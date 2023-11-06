#include <opencv2/core/hal/interface.h>
#include <opencv2/imgcodecs.hpp>
#include <nadjieb/mjpeg_streamer.hpp>

// Class to control streaming to the web
class StreamService {
public:
    // Function to begin streaming live camera feed to web server
    static void StartStream();

    // Function to stop the stream.
    static void StopStream();

    static void SendFrame(cv::Mat frame);
};