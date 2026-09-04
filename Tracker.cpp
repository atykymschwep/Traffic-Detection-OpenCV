#include "include/Tracker.hpp"
#include <cmath>
#include <algorithm>

// Constructor
Tracker::Tracker(int max_dist, int max_lost, int line_y, int line_x)
    : MAX_DISTANCE(max_dist), MAX_LOST_FRAMES(max_lost), 
      LINE_Y(line_y), LINE_X(line_x), 
      next_id(1), up_traffic_counter(0), left_traffic_counter(0) {}

void Tracker::update(const std::vector<cv::Rect>& boxes, const std::vector<int>& class_ids) {
    std::vector<cv::Rect> current_boxes;
    std::vector<cv::Point> current_centers;

    // Filter class IDs (keep vehicle classes only) and calculate center points
    for (size_t i = 0; i < boxes.size(); ++i) {
        int class_id = class_ids[i];
        // VisDrone vehicle class IDs: 3 (car), 4 (van), 5 (truck), 8 (bus)
        if (class_id != 3 && class_id != 4 && class_id != 5 && class_id != 8) continue;
        
        current_boxes.push_back(boxes[i]);
        current_centers.push_back(cv::Point(boxes[i].x + boxes[i].width / 2, boxes[i].y + boxes[i].height / 2));
    }

    std::vector<bool> used_detections(current_centers.size(), false);

    // Match previously active vehicles with new detections (nearest neighbor approach)
    for (auto& vehicle : active_vehicles) {
        int best_match_idx = -1;
        double min_dist = MAX_DISTANCE;

        for (size_t i = 0; i < current_centers.size(); ++i) {
            if (used_detections[i]) continue;

            double dist = std::hypot(vehicle.center.x - current_centers[i].x, 
                                     vehicle.center.y - current_centers[i].y);
            if (dist < min_dist) {
                min_dist = dist;
                best_match_idx = i;
            }
        }

        if (best_match_idx != -1) {
            cv::Point new_center = current_centers[best_match_idx];

            // Check if the vehicle crossed the horizontal line (moving UP)
            if (!vehicle.counted_up && vehicle.center.y > LINE_Y && new_center.y <= LINE_Y) {
                up_traffic_counter++;
                vehicle.counted_up = true;
            }
            // Check if the vehicle crossed the vertical line (moving LEFT)
            if (!vehicle.counted_left && vehicle.center.x > LINE_X && new_center.x <= LINE_X) {
                left_traffic_counter++;
                vehicle.counted_left = true;
            }

            vehicle.center = new_center;
            vehicle.box = current_boxes[best_match_idx]; // Update bounding box
            vehicle.lost_frames = 0;
            used_detections[best_match_idx] = true;
        } else {
            vehicle.lost_frames++;
        }
    }

    // Register new vehicles for unmatched detections
    for (size_t i = 0; i < current_centers.size(); ++i) {
        if (!used_detections[i]) {
            TrackedVehicle new_vehicle;
            new_vehicle.id = next_id++;
            new_vehicle.center = current_centers[i];
            new_vehicle.box = current_boxes[i];
            new_vehicle.lost_frames = 0;
            new_vehicle.counted_up = false;
            new_vehicle.counted_left = false;
            active_vehicles.push_back(new_vehicle);
        }
    }

    // Remove lost vehicles exceeding the maximum frame threshold
    active_vehicles.erase(std::remove_if(active_vehicles.begin(), active_vehicles.end(), [this](const TrackedVehicle& v) { return v.lost_frames > MAX_LOST_FRAMES; }),active_vehicles.end());
}

// Getters
const std::vector<TrackedVehicle>& Tracker::getActiveVehicles() const { return active_vehicles; }
int Tracker::getUpCount() const { return up_traffic_counter; }
int Tracker::getLeftCount() const { return left_traffic_counter; }
int Tracker::getLineY() const { return LINE_Y; }
int Tracker::getLineX() const { return LINE_X; }