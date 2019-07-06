# cmake needs this line
cmake_minimum_required(VERSION 3.1)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS "-O3")


# Define project name
project(opencv_example_project)

# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
find_package(OpenCV REQUIRED)
find_package(libuvc REQUIRED)


# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "libuvc library status:")
message(STATUS "    config: ${libuvc_DIR}")
message(STATUS "    version: ${libvc_VERSION}")
message(STATUS "    libraries: ${libuvc_LIBS}")
message(STATUS "    include path: ${libuvc_INCLUDE_DIRS}")

set(SOURCES OpenVideo.cpp cameraServer.cpp)
set(HEADERS OpenVideo.hpp)

file(GLOB cmaketest_HEADERS ${cmaketest_SRCDIR}/*.h)

include_directories(${CMAKE_SOURCE_DIR})

# Declare the executable target built from your sources
add_executable(opencv_cam_server ${SOURCES})

# Link your application with OpenCV libraries
target_link_libraries(opencv_cam_server ${OpenCV_LIBS}  ${libuvc_LIBS} /usr/local/lib/arm-linux-gnueabihf/libuvc.so)

