#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main() {
    std::string videoPath = "../traffic_video.mp4"; 
    cv::VideoCapture cap(videoPath);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video!" << std::endl;
        return -1;
    }

    // init bg subtraction
    cv::Ptr<cv::BackgroundSubtractorMOG2> bgSubtractor = cv::createBackgroundSubtractorMOG2();
    bgSubtractor->setShadowThreshold(0.10);

    cv::Mat frame, fgMask;
    
    // set up kernels for morphology
    cv::Mat smallKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::Mat largeKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9));

    while (true) {
        cap >> frame;
        if (frame.empty()) break; 

        cv::resize(frame, frame, cv::Size(1024, 576));

        // apply MOG2 and threshold out shadows
        bgSubtractor->apply(frame, fgMask);
        cv::threshold(fgMask, fgMask, 200, 255, cv::THRESH_BINARY);

        // remove noise and fill gaps
        cv::morphologyEx(fgMask, fgMask, cv::MORPH_OPEN, smallKernel);
        cv::morphologyEx(fgMask, fgMask, cv::MORPH_CLOSE, largeKernel);

        // find contours
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(fgMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (size_t i = 0; i < contours.size(); i++) {
            
            double area = cv::contourArea(contours[i]);

            // filter out objects that are too small or too large
            if (area < 400 || area > 15000) {
                continue; 
            }

            // draw final bounding boxes
            cv::Rect boundingRect = cv::boundingRect(contours[i]);
            cv::rectangle(frame, boundingRect, cv::Scalar(0, 255, 0), 2);
        }

        cv::imshow("Original Video", frame);
        cv::imshow("Clean Motion Mask", fgMask);

        // press ESC to exit
        if (cv::waitKey(30) == 27) break; 
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}