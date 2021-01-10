cmake_minimum_required (VERSION 3.0)
project(HA1)

# Prevent compilation in-source
if( ${CMAKE_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR} )
  Message( "" )
  Message( FATAL_ERROR "Source and build directories are the same!")
endif()

#Path of CImg.h file relative to the CMakeLists.txt
set(CIMG_H_PATH CImg-2.9.3_pre101320)

# Add Include directories for CImg (if you put it in non-default folders)
include_directories(${CIMG_H_PATH})

# Create executable as usual
file(GLOB SRC_FILES *.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES})

# CImg uses threads
target_link_libraries(${PROJECT_NAME} pthread)

# If CImg shall use built in png (for some reason ImageMagick does not work right now)
set(YOU_NEED_PNG 1)

if(${YOU_NEED_PNG} EQUAL 1) 
  
  message(STATUS "Looking for libpng...")

  # Flags to enable native support for PNG image files, using the PNG library.
  # ( http://www.libpng.org/ )
  set(CIMG_PNG_CCFLAGS -Dcimg_use_png)

  find_package(PNG REQUIRED)
  include_directories(${PNG_INCLUDE_DIR})
  target_link_libraries (${PROJECT_NAME} ${PNG_LIBRARY})
  target_compile_definitions(${PROJECT_NAME} PRIVATE cimg_use_png=1)

endif()