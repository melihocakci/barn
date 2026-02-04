cmake_minimum_required(VERSION 4.1.1)

project(barn LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG_TYPE} CONFIG_TYPE_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
endforeach()

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

add_library(steamworks_sdk SHARED IMPORTED)
target_include_directories(steamworks_sdk INTERFACE "lib/steamworks_sdk/public")
set(STEAM_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/steamworks_sdk/redistributable_bin")
if(WIN32)
    set_target_properties(steamworks_sdk PROPERTIES 
        IMPORTED_LOCATION "${STEAM_LIB_DIR}/win64/steam_api64.dll"
        IMPORTED_IMPLIB "${STEAM_LIB_DIR}/win64/steam_api64.lib"
    )
elseif(UNIX AND NOT APPLE)
    set_target_properties(steamworks_sdk PROPERTIES 
        IMPORTED_LOCATION "${STEAM_LIB_DIR}/linux64/libsteam_api.so"
    )
elseif(APPLE)
    set_target_properties(steamworks_sdk PROPERTIES 
        IMPORTED_LOCATION "${STEAM_LIB_DIR}/osx/libsteam_api.dylib"
    )
endif()

add_executable(${PROJECT_NAME} WIN32
    "src/main.cpp"
    "src/scenes.cpp"
    "src/systems.cpp"
    "src/definitions.cpp"
    "src/factories.cpp"
    "src/utils.cpp"
    "src/config.cpp"
)

target_link_libraries(${PROJECT_NAME} PRIVATE "SDL3::SDL3" "SDL3_image::SDL3_image" "EnTT::EnTT" "box2d" "steamworks_sdk")

get_target_property(STEAM_LIB steamworks_sdk IMPORTED_LOCATION)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${STEAM_LIB} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
)
