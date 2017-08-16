cmake_minimum_required(VERSION 3.8)
project(mrain VERSION 0.1.0 LANGUAGES CXX)


add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE .)

target_sources(${PROJECT_NAME} PRIVATE
  app_window.cpp
  application.cpp
  base_capture.cpp
  blas.cpp
  gl_renderer.cpp
  gpu_program.cpp
  main.cpp
  matrix.cpp
  options.cpp
  scene.cpp
  stuff.cpp
  texture_atlas.cpp
  transform.cpp
  video_buffer.cpp
)

if(UNIX)
  include(./nix/CMakeLists.txt)
elseif(WIN32)
  include(./win/CMakeLists.txt)
endif()

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Interactive screensaver with webcam integration")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "mrain")
set(CPACK_PACKAGE_CONTACT "pavel.karneliuk@gmail.com")
include(CPack)