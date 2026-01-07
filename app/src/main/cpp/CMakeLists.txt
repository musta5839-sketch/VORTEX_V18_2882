
cmake_minimum_required(VERSION 3.22.1)
add_library(native-lib SHARED native_engine.cpp game_logic.cpp)
find_library(log-lib log)
find_library(gles-lib GLESv3)
find_library(egl-lib EGL)
find_library(sl-lib OpenSLES)
target_link_libraries(native-lib ${log-lib} ${gles-lib} ${egl-lib} ${sl-lib})
