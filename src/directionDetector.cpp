// g++ -std=c++17 `pkg-config --cflags --libs opencv4` directionDetector.cpp -o DirectionDetector
// ./DirectionDetector

#include <iostream>
// #include <ctype.h>

#include "opencv2/core/utility.hpp"
// #include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
// #include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

// using namespace cv;

cv::Mat image;

cv::Point origin;
cv::Rect selection;

bool selectObject = false;
int trackObject = 0;
int vMin = 10, vMax = 256, sMin = 30;

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

int main()
{
    std::cout << "Controls:\nLeft click drag to select tracking object\nC to clear tracker\nP to pause\nEsc to quit";

    cv::VideoCapture capture;
    cv::Rect trackWindow;

    capture.open(0);

    bool paused = false;
    int hSize = 16;
    float hRanges[] = {0, 180};
    const float* phRanges = hRanges;

    cv::Mat frame, hsv, hue, mask, hist, backProj;
    cv::Mat histImg = cv::Mat::zeros(200, 320, CV_8UC3);

    while (true)
    {
        if (!paused)
        {
            capture >> frame;
            if (frame.empty()) break;
        }

        frame.copyTo(image);

        if (!paused)
        {
            cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

            if (trackObject)
            {
                if (trackObject < 0)
                {
                    cv::Mat roi(hue, selection), maskroi(mask, selection);
                    cv::calcHist(&roi, 1, 0, maskroi, hist, 1, &hSize, &phRanges);
                    normalize(hist, hist, 0, 255, cv::NORM_MINMAX);

                    trackWindow = selection;
                    trackObject = 1;
                    histImg = cv::Scalar::all(0);
                    int binW = histImg.cols / hSize;

                    cv::Mat buf(1, hSize, CV_8UC3);
                    for (int i = 0; i < hSize; i++)
                    {
                        buf.at<cv::Vec3b>(i) = cv::Vec3b(cv::saturate_cast<uchar>(i * 180.0 / hSize), 255, 255);
                    }
                    cv::cvtColor(buf, buf, cv::COLOR_HSV2BGR);
                }

                cv::RotatedRect trackBox;
                cv::ellipse(image, trackBox, cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
            }
        }
        else if (trackObject < 0) paused = false;

        imshow("Camera", image);

        char ch = (char)cv::waitKey(10);
        if (ch == 27) break;
        switch (ch)
        {
            case 'c':
                trackObject = 0;
                break;
            case 'p':
                paused = !paused;
                break;
        }
    }
}