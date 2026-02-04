cmake_minimum_required(VERSION 3.28)
project(mygame LANGUAGES CXX)

add_subdirectory(lib/SFML)

add_executable(${PROJECT_NAME} src/main.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics)
