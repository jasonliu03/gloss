cmake_minimum_required (VERSION 2.6)


set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/libs/opencv-install/lib/pkgconfig")
find_package(PkgConfig REQUIRED)
pkg_search_module(PKG_OPENCV REQUIRED opencv) 


INCLUDE_DIRECTORIES(./src)


SET(LIB_MSRCR_SOURCE 
	./src/MSRCR.cpp
	./src/MSRCR.h)
ADD_LIBRARY(msrcr SHARED ${LIB_MSRCR_SOURCE})
TARGET_LINK_LIBRARIES(msrcr
	${OpenCV_LIBS})

SET(LIB_MSRCR msrcr)

add_executable(gloss ./src/main.cpp)

include_directories(${PKG_OPENCV_INCLUDE_DIRS})
target_link_libraries (gloss ${LIB_MSRCR} ${PKG_OPENCV_LDFLAGS})

