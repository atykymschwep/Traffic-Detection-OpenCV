#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <vector>
#include "include/Tracker.hpp" // Include our smart tracker

// DRAWING FUNCTION: Handles all graphics
void drawOverlay(cv::Mat& frame, const Tracker& tracker) {

    // 1. Draw vehicle bounding boxes
    for (const auto& vehicle : tracker.getActiveVehicles()) {
        if (vehicle.lost_frames > 0) continue; // Do not draw if the vehicle is temporarily lost

        // Green box if the vehicle has already crossed the line, otherwise yellow
        bool is_counted = vehicle.counted_up || vehicle.counted_left;
        cv::Scalar color = is_counted ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 255, 255);

        cv::rectangle(frame, vehicle.box, color, 2);
        
        std::string label = "ID: " + std::to_string(vehicle.id);
        int baseLine;

        cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.6, 2, &baseLine);

        cv::rectangle(frame, cv::Point(vehicle.box.x, vehicle.box.y - labelSize.height - 5),
                      cv::Point(vehicle.box.x + labelSize.width, vehicle.box.y), color, cv::FILLED);

        cv::putText(frame, label, cv::Point(vehicle.box.x, vehicle.box.y - 5),
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 0), 2);
    }

    // 2. Draw counting lines
    cv::line(frame, cv::Point(0, tracker.getLineY()), cv::Point(frame.cols, tracker.getLineY()), cv::Scalar(0, 0, 255), 2);
    cv::line(frame, cv::Point(tracker.getLineX(), 0), cv::Point(tracker.getLineX(), frame.rows), cv::Scalar(255, 0, 255), 2);

    // 3. Draw statistics dashboard
    cv::rectangle(frame, cv::Point(15, 15), cv::Point(330, 95), cv::Scalar(0, 0, 0), cv::FILLED);

    cv::putText(frame, "Vehicles UP: " + std::to_string(tracker.getUpCount()), cv::Point(25, 45), 
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);

    cv::putText(frame, "Vehicles LEFT: " + std::to_string(tracker.getLeftCount()), cv::Point(25, 80), 
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 0, 255), 2);
}

// MAIN PROGRAM
int main() {
    std::string videoPath = "../traffic_video.mp4";
    cv::VideoCapture cap(videoPath);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video!" << std::endl;
        return -1;
    }

    std::string modelPath = "../visdrone_small.onnx";
    cv::dnn::Net net = cv::dnn::readNetFromONNX(modelPath);
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

    const float CONFIDENCE_THRESHOLD = 0.20f; 
    const float NMS_THRESHOLD = 0.35f;        

    cv::Mat frame;

    // Initialize our smart tracker (distance=60, lost_frames=10, Y=400, X=345)
    Tracker tracker(60, 10, 400, 345);

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cv::resize(frame, frame, cv::Size(1024, 576));

        // NEURAL NETWORK DETECTION
        cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0 / 255.0, cv::Size(640, 640), cv::Scalar(), true, false);
        net.setInput(blob);

        std::vector<cv::Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());

        cv::Mat output = outputs[0];
        
        // Save dimensions before reshaping the matrix
        int rows = output.size[1];       
        int dimensions = output.size[2]; 

        if (output.dims == 3) {
            output = output.reshape(1, rows); // Now the matrix is 2D
        }

        float x_factor = frame.cols / 640.0f;
        float y_factor = frame.rows / 640.0f;

        std::vector<int> class_ids;
        std::vector<float> confidences;
        std::vector<cv::Rect> boxes;

        // Parse neural network output
        for (int i = 0; i < dimensions; ++i) {
            cv::Mat scores = output.col(i).rowRange(4, rows);
            cv::Point class_id_point;
            double max_score;
            cv::minMaxLoc(scores, nullptr, &max_score, nullptr, &class_id_point);

            if (max_score >= CONFIDENCE_THRESHOLD) {
                float cx = output.at<float>(0, i);
                float cy = output.at<float>(1, i);
                float w  = output.at<float>(2, i);
                float h  = output.at<float>(3, i);

                boxes.push_back(cv::Rect(
                    static_cast<int>((cx - 0.5f * w) * x_factor),
                    static_cast<int>((cy - 0.5f * h) * y_factor),
                    static_cast<int>(w * x_factor),
                    static_cast<int>(h * y_factor)
                ));
                confidences.push_back(static_cast<float>(max_score));
                class_ids.push_back(class_id_point.y);
            }
        }

        // Filter duplicates (Non-Maximum Suppression)
        std::vector<int> indices;
        cv::dnn::NMSBoxes(boxes, confidences, CONFIDENCE_THRESHOLD, NMS_THRESHOLD, indices);

        // Create final lists of boxes and IDs after NMS cleanup
        std::vector<cv::Rect> final_boxes;
        std::vector<int> final_class_ids;
        for (int idx : indices) {
            final_boxes.push_back(boxes[idx]);
            final_class_ids.push_back(class_ids[idx]);
        }

        // TRACKER PROCESSING
        tracker.update(final_boxes, final_class_ids);

        // DRAWING AND DISPLAY
        drawOverlay(frame, tracker);

        cv::imshow("VisDrone Traffic Detection", frame);
        if (cv::waitKey(1) == 27) break; // Exit on ESC key
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}