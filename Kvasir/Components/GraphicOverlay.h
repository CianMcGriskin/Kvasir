#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

class GraphicOverlay {
public:
    // Draws a graphical box over what the model is detecting
    static void DrawBox(float* output, int numPerDetect, int i, cv::Mat image);

private:

};
