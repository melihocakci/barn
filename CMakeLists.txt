cmake_minimum_required(VERSION 4.1)

project(barn LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/SDL EXCLUDE_FROM_ALL)

set(SDL3_DIR "${CMAKE_CURRENT_BINARY_DIR}/lib/SDL" CACHE PATH "" FORCE)

set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_WEBP OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_TIF OFF CACHE BOOL "" FORCE)
set(SDL_IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_IMAGE_VENDORED ON CACHE BOOL "" FORCE)
add_subdirectory(lib/SDL_image EXCLUDE_FROM_ALL)

#set(SDL_MIXER_SAMPLES OFF CACHE BOOL "" FORCE)
#set(SDL_MIXER_VENDORED ON CACHE BOOL "" FORCE)
#add_subdirectory(lib/SDL_mixer EXCLUDE_FROM_ALL)

add_subdirectory(lib/entt EXCLUDE_FROM_ALL)

add_subdirectory(lib/box2d EXCLUDE_FROM_ALL)

add_library(steamworks_sdk INTERFACE EXCLUDE_FROM_ALL)
target_include_directories(steamworks_sdk INTERFACE lib/steamworks_sdk/public)
if(WIN32)
    set(STEAM_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/steamworks_sdk/redistributable_bin/win64)
    target_link_libraries(steamworks_sdk INTERFACE ${STEAM_LIB_DIR}/steam_api64.lib)
    file(COPY ${STEAM_LIB_DIR}/steam_api64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

add_executable(${PROJECT_NAME}
    "src/main.cpp"
    "src/scenes.cpp"
    "src/systems.cpp"
    "src/definitions.cpp"
    "src/factories.cpp"
    "src/utils.cpp"
    "src/config.cpp"
)

target_link_libraries(${PROJECT_NAME} PRIVATE "SDL3::SDL3" "SDL3_image::SDL3_image" "EnTT::EnTT" "box2d")
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
