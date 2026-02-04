cmake_minimum_required(VERSION 3.28)
project(barn LANGUAGES CXX)

add_subdirectory(lib/SFML)

add_subdirectory(lib/entt)

add_subdirectory(lib/box2d)

add_library(steamworks_sdk INTERFACE)
target_include_directories(steamworks_sdk INTERFACE lib/steamworks_sdk/public)
if(WIN32)
    set(STEAM_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/steamworks_sdk/redistributable_bin/win64)
    target_link_libraries(steamworks_sdk INTERFACE ${STEAM_LIB_DIR}/steam_api64.lib)
    file(COPY ${STEAM_LIB_DIR}/steam_api64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

add_executable(${PROJECT_NAME} "src/main.cpp" "src/scenes.cpp" "src/systems.cpp" "src/definitions.cpp" "src/factories.cpp" "src/assets.cpp")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_link_libraries(${PROJECT_NAME} PRIVATE steamworks_sdk EnTT::EnTT box2d SFML::Graphics SFML::Audio)
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    target_link_libraries(${PROJECT_NAME} PRIVATE sfml-main)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
