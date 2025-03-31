cmake_minimum_required(VERSION 3.28)
project(mygame LANGUAGES CXX)

add_subdirectory(lib/SFML)

add_subdirectory(lib/entt)

add_library(steamworks_sdk INTERFACE)
target_include_directories(steamworks_sdk INTERFACE lib/steamworks_sdk/public)
if(WIN32)
    set(STEAM_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/steamworks_sdk/redistributable_bin/win64)
    target_link_libraries(steamworks_sdk INTERFACE ${STEAM_LIB_DIR}/steam_api64.lib)
    file(COPY ${STEAM_LIB_DIR}/steam_api64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

add_executable(mygame src/main.cpp)
target_compile_features(mygame PRIVATE cxx_std_23)
target_link_libraries(mygame PRIVATE steamworks_sdk EnTT::EnTT SFML::Graphics SFML::Audio)
if(WIN32)
    set_target_properties(mygame PROPERTIES WIN32_EXECUTABLE TRUE)
    target_link_libraries(mygame PRIVATE sfml-main)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
