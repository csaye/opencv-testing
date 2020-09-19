// g++ -std=c++17 `pkg-config --cflags --libs opencv4` directionDetector.cpp -o DirectionDetector
// ./DirectionDetector

// #include <iostream>
// #include <ctype.h>

#include "opencv2/core/utility.hpp"
// #include "opencv2/video/tracking.hpp"
// #include "opencv2/imgproc.hpp"
// #include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

// using namespace cv;

bool selectObject = false;
int trackObject = 0;

cv::Mat image;

cv::Point origin;
cv::Rect selection;

static void onMouse(int event, int x, int y)
{
    if (selectObject)
    {
        selection.x = MIN(x, origin.x);
        selection.y = MIN(y, origin.y);

        selection.width = std::abs(x - origin.x);
        selection.height = std::abs(y - origin.y);

        selection &= cv::Rect(0, 0, image.cols, image.rows);
    }

    switch (event)
    {
        case cv::EVENT_LBUTTONDOWN:
            origin = cv::Point(x, y);
            selection = cv::Rect(x, y, 0, 0);
            selectObject = true;
            break;
        case cv::EVENT_LBUTTONUP:
            selectObject = false;
            if (selection.width > 0 && selection.height > 0) trackObject = -1;
            break;
    }
}

int main(int argc, const char** argv)
{
    
}