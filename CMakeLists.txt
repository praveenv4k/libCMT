project(libCMT)

cmake_minimum_required(VERSION 2.6)
find_package(OpenCV REQUIRED)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(KINECT_INCLUDE $ENV{KINECTSDK20_DIR}inc)
include_directories(${KINECT_INCLUDE})

set(KINECT_LIBRARY $ENV{KINECTSDK20_DIR}Lib\\x86\\Kinect20.lib)

option(BUILD_MAIN "Build main" ON)
option(BUILD_WITH_QT "Use QDebug" OFF)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

add_library(cmt CMT.cpp)
add_library(kinectvideocapture KinectVideoCapture.cpp)
target_link_libraries(cmt ${OpenCV_LIBS})
target_link_libraries(kinectvideocapture ${OpenCV_LIBS} ${KINECT_LIBRARY})

if(BUILD_MAIN)
	add_executable(main main.cpp)
	if(BUILD_WITH_QT)
		find_package(Qt4 REQUIRED)

		include(${QT_USE_FILE})
		add_definitions(${QT_DEFINITIONS} -DDEBUG_MODE)

		target_link_libraries(main cmt kinectvideocapture ${OpenCV_LIBS} ${QT_LIBRARIES})
	else()
		target_link_libraries(main cmt kinectvideocapture ${OpenCV_LIBS} ${KINECT_LIBRARY})
	endif()
endif()

