# CMakeList.txt : CMake project for Kvasir, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(OpenCV_DIR "./Dependencies/opencv/build")

find_package(OpenCV REQUIRED)


# Add source to this project's executable.
add_executable (Kvasir "Kvasir.cpp" "Kvasir.h" "Components/CameraConnection.cpp" "Components/CameraConnection.h" "Components/Model.cpp" "Components/Model.h")

target_link_libraries(Kvasir PRIVATE ${OpenCV_LIBS})


# TODO: Add tests and install targets if needed.
