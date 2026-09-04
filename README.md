# 🚗 Traffic Detection with OpenCV (C++)

Real-time vehicle detection, tracking, and traffic counting using **OpenCV DNN** and a **VisDrone ONNX model**.

## ✨ Features

* **Vehicle Detection** — detects vehicles using a VisDrone ONNX model.
* **Vehicle Filtering** — detects only cars, vans, trucks, and buses.
* **Non-Maximum Suppression** — removes duplicate detections.
* **Vehicle Tracking** — assigns a unique ID to each vehicle and tracks it between frames.
* **Traffic Counting** — counts vehicles moving **UP** and **LEFT** across counting lines.
* **Real-Time Visualization** — displays vehicle bounding boxes, IDs, counting lines, and traffic statistics.

## 🛠 Requirements

* C++17
* OpenCV 4.x
* CMake 3.10+
* OpenCV DNN support

## 🚀 Build & Run

### 1. Clone the repository

```bash
git clone https://github.com/atykymschwep/Traffic-Detection-OpenCV.git

cd Traffic-Detection-OpenCV
```

### 2. Add the video and model

Place the required files in the project root:

```text
traffic_video.mp4
visdrone_small.onnx
```

### 3. Build

```bash
mkdir build
cd build

cmake ..
make
```

### 4. Run

```bash
./TrafficCounter
```

Press **ESC** to exit.

## 📷 Example

The application displays detected vehicles with bounding boxes and unique IDs, counting lines, and real-time traffic statistics.

<img width="1017" height="576" alt="Vehicle detection and tracking" src="https://github.com/user-attachments/assets/9ee16d52-628f-4068-8844-a62b04b8d8bc" />

Vehicles are counted when they cross the configured horizontal or vertical line.

<img width="1017" height="576" alt="Traffic counting" src="https://github.com/user-attachments/assets/a6bbeb5b-956b-42f0-8aa2-226876f90c20" />

## 📊 Tracking & Counting

The tracker uses a nearest-neighbor approach to match vehicles between frames.

Default parameters:

```text
Maximum tracking distance: 60 px
Maximum lost frames:       10
Horizontal line (UP):      Y = 400
Vertical line (LEFT):      X = 345
```

A vehicle is counted only once for each direction.

## 🎯 Detection

```text
Confidence threshold: 0.20
NMS threshold:         0.35
```

## 📚 Technologies

* **C++17**
* **OpenCV**
* **OpenCV DNN**
* **CMake**
* **VisDrone ONNX**
* **Nearest-Neighbor Tracking**
