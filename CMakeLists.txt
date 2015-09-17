cmake_minimum_required(VERSION 3.0)

project(Mural)

# Enable C++11
if (APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
elseif (WIN32 OR UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# Depencencies
# - Libs
find_library(BOOST_FILESYSTEM_LIB boost_filesystem ${PROJECT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME})
if (NOT BOOST_FILESYSTEM_LIB)
  message(FATAL_ERROR "Boost filesystem lib not found")
endif()
find_library(BOOST_SYSTEM_LIB boost_system ${PROJECT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME})
if (NOT BOOST_SYSTEM_LIB)
  message(FATAL_ERROR "Boost system lib not found")
endif()
find_library(BOOST_DATE_TIME_LIB boost_date_time ${PROJECT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME})
if (NOT BOOST_DATE_TIME_LIB)
  message(FATAL_ERROR "Boost datetime lib not found")
endif()
set(BOOST_LIB
  ${BOOST_FILESYSTEM_LIB}
  ${BOOST_SYSTEM_LIB}
  ${BOOST_DATE_TIME_LIB}
)

find_library(GLFW_LIB glfw3 ${PROJECT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME})
if (NOT GLFW_LIB)
  message(FATAL_ERROR "GLFW lib not found")
endif()
find_library(Cocoa_FRAMEWORK Cocoa)
find_library(OpenGL_FRAMEWORK OpenGL)
find_library(IOKit_FRAMEWORK IOKit)
find_library(CoreVideo_FRAMEWORK CoreVideo)

add_library(glew STATIC ${PROJECT_SOURCE_DIR}/Source/glew.c)

set(Mural_LIBS
  ${BOOST_LIB}
  ${GLFW_LIB} ${Cocoa_FRAMEWORK} ${OpenGL_FRAMEWORK} ${IOKit_FRAMEWORK} ${CoreVideo_FRAMEWORK}
  glew
)

# Core sources
set(Mural_SOURCES
  Source/AppController.cc
  Source/MuSharedOpenGLContext.cc
  Source/MuTimer.cc

  Source/MuCanvas/2D/MuGLProgram2D.cc

  Source/main.cc
)

# Program
include_directories(
  ${PROJECT_SOURCE_DIR}/Source
  ${PROJECT_SOURCE_DIR}/Source/glm
)
add_executable(mural ${Mural_SOURCES})
target_link_libraries(mural ${Mural_LIBS})
