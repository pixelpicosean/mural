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

find_library(Z_LIB z ${PROJECT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME})
if (NOT Z_LIB)
  message(FATAL_ERROR "Z lib not found")
endif()

find_library(GLFW_LIB glfw3 ${PROJECT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME})
if (NOT GLFW_LIB)
  message(FATAL_ERROR "GLFW lib not found")
endif()
find_library(Cocoa_FRAMEWORK Cocoa)
find_library(OpenGL_FRAMEWORK OpenGL)
find_library(IOKit_FRAMEWORK IOKit)
find_library(CoreVideo_FRAMEWORK CoreVideo)

set(Mural_LIBS
  ${BOOST_LIB}
  ${Z_LIB}
  ${GLFW_LIB} ${Cocoa_FRAMEWORK} ${OpenGL_FRAMEWORK} ${IOKit_FRAMEWORK} ${CoreVideo_FRAMEWORK}
)

# - Sources
set(Linebreak_SOURCES
  src/linebreak/linebreak.c
  src/linebreak/linebreakdata.c
  src/linebreak/linebreakdef.c
)

set(NanoVG_SOURCES
  src/nanovg/nanovg.c
)

# Core sources
set(Mural_SOURCES
  ${Linebreak_SOURCES}
  ${NanoVG_SOURCES}

  src/MuArea.cc
  src/MuAssetsManager.cc
  src/MuBuffer.cc
  src/MuCore.cc
  src/MuDataSource.cc
  src/MuDataTarget.cc
  src/MuEvent.cc
  src/MuEventDispatcher.cc
  src/MuFbo.cc
  src/MuInputEvent.cc
  src/MuNanoVG.cc
  src/MuRect.cc
  src/MuStream.cc
  src/MuSurface.cc
  src/MuTimer.cc
  src/MuUnicode.cc
  src/MuUrl.cc
  src/MuUtilities.cc
  src/perf.c

  src/main.cc
)

# Program
include_directories(${PROJECT_SOURCE_DIR}/src)
add_executable(mural ${Mural_SOURCES})
target_link_libraries(mural ${Mural_LIBS})
