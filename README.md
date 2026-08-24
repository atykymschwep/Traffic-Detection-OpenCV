# 🚗 Traffic Detection with OpenCV (C++)

A lightweight C++ application for real-time vehicle detection using **OpenCV**, background subtraction, and morphological image processing.

This project demonstrates a classical computer vision approach to traffic detection. It works well in suitable conditions, but it is not a perfect vehicle detector and may produce false positives or miss vehicles depending on lighting, shadows, camera movement, and scene complexity.

## ✨ Features

* **MOG2 Background Subtraction** — detects moving vehicles.
* **Morphological Filtering** — removes noise using `MORPH_OPEN` and `MORPH_CLOSE`.
* **Contour Detection** — filters objects by bounding box area.
* **Real-Time Visualization** — shows the original video and binary motion mask side by side.

## 🛠 Requirements

* C++17
* OpenCV 4.x
* CMake 3.10+

## 🚀 Build & Run

### 1. Clone the repository

```bash
git clone https://github.com/atykymschwep/Traffic-Detection-OpenCV.git
cd Traffic-Detection-OpenCV
```


### 2. Add the video

Place your traffic video in the project root:

```text
traffic_video.mp4
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
./traffic_detection
```

## 📷 Example

The application displays:

* detected vehicles with bounding boxes
* <img width="1017" height="576" alt="Screenshot from 2026-08-24 20-59-47" src="https://github.com/user-attachments/assets/9ee16d52-628f-4068-8844-a62b04b8d8bc" />
* the binary motion mask
* <img width="1017" height="576" alt="Screenshot from 2026-08-24 20-59-31" src="https://github.com/user-attachments/assets/a6bbeb5b-956b-42f0-8aa2-226876f90c20" />



## 📚 Technologies

* **C++17**
* **OpenCV**
* **CMake**
* **MOG2 Background Subtraction**
