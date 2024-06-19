cmake_minimum_required(VERSION 3.10)

project(gurl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

function(configure_platform)
  if(WIN32)
    set(CMAKE_CXX_FLAGS "/O2 /EHsc")
  else()
    set(CMAKE_CXX_FLAGS "-O3")
  endif()
endfunction()

configure_platform()

add_executable(gurl gurl.cpp)

target_compile_features(gurl PRIVATE cxx_std_17)