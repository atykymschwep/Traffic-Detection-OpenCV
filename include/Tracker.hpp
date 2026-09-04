#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

// Structure representing a tracked vehicle in memory
struct TrackedVehicle {
    int id;               // Unique id for the vehicle
    cv::Point center;     // Current center coordinates of the vehicle
    cv::Rect box;         // Current bounding box
    int lost_frames;      // Number of consecutive frames the vehicle was not detected
    bool counted_up;      // Flag to prevent double counting when crossing the horizontal line
    bool counted_left;    // Flag to prevent double counting when crossing the vertical line
};

class Tracker {
public:
    // Constructor
    Tracker(int max_dist = 60, int max_lost = 10, int line_y = 400, int line_x = 345);

    // Main update function: processes neural network detections and handles counting logic
    void update(const std::vector<cv::Rect>& boxes, const std::vector<int>& class_ids);

    // Getter functions to retrieve data for visualization in main.cpp
    const std::vector<TrackedVehicle>& getActiveVehicles() const;
    int getUpCount() const;
    int getLeftCount() const;
    int getLineY() const;
    int getLineX() const;

private:
    int next_id;                                  // ID to be assigned to the next new vehicle
    std::vector<TrackedVehicle> active_vehicles;  // List of currently tracked vehicles
    
    int MAX_DISTANCE;       // Maximum allowed distance between frames to consider it the same vehicle
    int MAX_LOST_FRAMES;    // Number of frames before a lost vehicle is removed from memory
    int LINE_Y;             // Y-coordinate of the horizontal counting line
    int LINE_X;             // X-coordinate of the vertical counting line
    
    int up_traffic_counter;     // Counter for vehicles moving up
    int left_traffic_counter;   // Counter for vehicles moving left
};