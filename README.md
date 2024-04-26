# Kvasir

**Kvasir Backend** is a project that is designed for IoT devices to run as a modern CCTV camera system.
This applciation can be executed on WIndows & Linux. Mac OS is untested. The project is built using CMake and using make (on Linux) or cmake .. --build (using Windows).

# Table of Contents
1. [Features](#features)
2. [How it works](#how-it-works)
3. [Build Guide](#build-guide)
4. [Demonstration](#demonstration)
5. [Windows Build](#windows-build)

# Features
 - Saves video based on motion detection, stops recording if no motion detection after 10 seconds.
 - Automatic video uploads to the cloud (specificity AWS S3) and removed from the local machine.
 - Live detection and recognition of peoples faces.
 - Notifies the user (via Kvasir Mobile) if a person that is on the list of saved faces has been detected within the camera frame.
 - Processes faces sent (via **Kvasir Mobile**) to Kvasir Backend and adds the face data to the saved faces list.
 - Sends a notification (via **Kvasir Mobile**) displaying the confidence along with the assigned name of the person to the user.
 - Provides live streaming to the user (via **Kvasir Mobile**) along with previously recorded videos

## How It Works


 - The list of saved faces is a JSON file containing information such as "Name", "Reason" along with a face array which consists of all face data belonging to that individual, meaning one individual could have multiple sets of data to detect that one person. This significantly increases detection rate.
 - This project uses a face detection model to crop the faces, then uses FaceNet to run inference on the cropped images to gather face embeddings, which is the face data of a person.
 - Faces are cropped by the user gathered from previous footage from the camera (via **Kvasir Mobile**). This is because FaceNet works best coming from the same angle and similar lighting scenarios, since this is a CCTV camera system, it makes sense that images are taken directly from the CCTV camera itself.
 - Videos are saved every minute and uploaded to the cloud they are then deleted from the local system.
 - Videos stop and start based on motion detection - this is to reduce wasted disk memory. After motion is not detected, the segment will stop and automatically upload to S3 followed by being delete from the local file system.

## Build Guide

This project has many dependencies such as **Tensorflow Lite**, **OpenCV**, **nlohmann/json**, **AWS-SDK** 
(For C++ - S3 specifically) and **nadjieb/cpp-mjpeg-streamer**.

To clone the repository make sure to include the branch argument.
``git clone --branch ModelInference https://github.com/CianMcGriskin/Kvasir.git``

To download these dependencies please use naviage to the Kvasir/Scripts and run the python script named "download-dependencies.py". After this run "build-dependencies.py". It is recommended to use the following versions of each dependence based on their commit hash:
| Repo | Git Hash |
|--|--|
| OpenCV | 7b6d65cf201106de5cd9b3bebdcc939da115b654 |
|TensorFow Lite|068bee993d59252a46c504d17162486218f3bc82 |
|cpp-mjpeg-streamer|65e2129954dbbdb3b2b227c3d76bdb1283e88855|
|aws-sdk-cpp	| d1fa2bd0ac80e3b3fdde0e5b3a1f961d3f51f968 |
|nlohmann/json|f56c6e2e30241b9245161a86ae9fecf6543bf411|

## Demonstration

Video demo is located at Demonstration.wmv


## Windows Build
For building on windows you will need to add the following code to the start of ./Kvasir/CMakeLists.txt underneath project(Kvasir). You will need to alter the variable CMAKE_ASM_COMPILER to your own version of a compiler (Visual Studio in this case), remember this is designed with C++ 17.
```markdown
set(CMAKE_ASM_COMPILER "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.38.33130/bin/Hostx64/x64/cl.exe") 
set(AWSSDK_DIR "./Dependencies/aws-sdk-cpp/build/install/lib/cmake/AWSSDK")
set(aws-cpp-sdk-core_DIR "./Dependencies/aws-sdk-cpp/build/install/lib/cmake/aws-cpp-sdk-core") 
set(aws-crt-cpp_DIR "./Dependencies/aws-sdk-cpp/build/install/lib/aws-crt-cpp/cmake") 
set(aws-cpp-sdk-s3_DIR "./Dependencies/aws-sdk-cpp/build/install/lib/cmake/aws-cpp-sdk-s3")

