# CMakeList.txt : CMake project for Kvasir, include source and define
#

cmake_minimum_required(VERSION 3.8)
project(Kvasir)

# Set the path to OpenCV build directory
set(OpenCV_DIR "./Dependencies/OpenCV/build")
find_package(OpenCV REQUIRED)

set(TENSORFLOW_SOURCE_DIR "" CACHE PATH
  "Directory that contains the TensorFlow project" )
#find_package(tensorflow-lite REQUIRED)

message(PROJECT_SOURCE_DIR="${TENSORFLOW_SOURCE_DIR}")

# Include TensorFlow lite library
add_subdirectory(
  "./Dependencies/Tensorflow/tensorflow/lite"
  "${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite" EXCLUDE_FROM_ALL)

# Add source to this project's executable.
add_executable(Kvasir "Kvasir.cpp" "Kvasir.h" "Components/CameraConnection.cpp" "Components/CameraConnection.h" "Components/Model.cpp" "Components/Model.h"
        Components/CameraControl.cpp
        Components/CameraControl.h
        Components/GraphicOverlay.cpp
        Components/GraphicOverlay.h)

# Link TensorFlow lite library
target_link_libraries(Kvasir tensorflow-lite)

# Link OpenCV libraries
target_link_libraries(Kvasir ${OpenCV_LIBS})

# TODO: Add tests and install targets if needed.
