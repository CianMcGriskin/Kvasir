# CMakeList.txt : CMake project for Kvasir, include source and define
#

cmake_minimum_required(VERSION 3.8)
project(Kvasir)

# Set the path to OpenCV build directory
set(OpenCV_DIR "./Dependencies/OpenCV/build")
find_package(OpenCV REQUIRED)

set(TENSORFLOW_SOURCE_DIR "" CACHE PATH
  "Directory that contains the TensorFlow project" )

message(PROJECT_SOURCE_DIR="${TENSORFLOW_SOURCE_DIR}")

# Include TensorFlow lite library
add_subdirectory(
  "./Dependencies/Tensorflow/tensorflow/lite"
  "${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite" EXCLUDE_FROM_ALL)
  
add_subdirectory(
  "./Dependencies/cpp-mjpeg-streamer"
  "${CMAKE_CURRENT_BINARY_DIR}/cpp-mjpeg-streamer-build")
  
add_subdirectory(
  "./Dependencies/json"
  "${CMAKE_CURRENT_BINARY_DIR}/json")
  
# Add source to this project's executable.
add_executable(Kvasir "Kvasir.cpp" "Kvasir.h" "Components/Model.cpp" "Components/Model.h"
        Components/CameraControl.cpp
        Components/CameraControl.h
        Components/WebserverCommunication/StreamService.cpp
        Components/WebserverCommunication/StreamService.h
        Components/FaceDetection/FaceDetection.cpp
        Components/FaceDetection/FaceDetection.h
        Components/FaceStorage/FaceStorage.cpp
        Components/FaceStorage/FaceStorage.h
        Components/Utils/Utils.cpp
        Components/Utils/Utils.h)

# Link TensorFlow lite library
target_link_libraries(Kvasir tensorflow-lite)

# Link OpenCV libraries
target_link_libraries(Kvasir tensorflow-lite ${OpenCV_LIBS})

target_link_libraries(Kvasir nadjieb_mjpeg_streamer::nadjieb_mjpeg_streamer)

target_link_libraries(Kvasir nlohmann_json::nlohmann_json)
