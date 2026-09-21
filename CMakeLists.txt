cmake_minimum_required(VERSION 3.28)

project(barn LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    add_compile_options(/experimental:external /external:W0 /external:anglebrackets)
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/SDL EXCLUDE_FROM_ALL)
set(SDL3_DIR "${CMAKE_CURRENT_BINARY_DIR}/lib/SDL" CACHE PATH "" FORCE)

set(SDLIMAGE_ANI OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_BMP OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_GIF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_JXL OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_LBM OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_PCX OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_PNM OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_QOI OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_SVG OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_TGA OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_TIF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_WEBP OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_XCF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_XPM OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_XV OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_PNG_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_IMAGE_VENDORED ON CACHE BOOL "" FORCE)
add_subdirectory(lib/SDL_image EXCLUDE_FROM_ALL)

set(SDLMIXER_AIFF OFF CACHE BOOL "" FORCE)
set(SDLMIXER_VOC OFF CACHE BOOL "" FORCE)
set(SDLMIXER_AU OFF CACHE BOOL "" FORCE)
set(SDLMIXER_FLAC OFF CACHE BOOL "" FORCE)
set(SDLMIXER_GME OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MOD OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MP3 OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MIDI OFF CACHE BOOL "" FORCE)
set(SDLMIXER_OPUS OFF CACHE BOOL "" FORCE)
set(SDLMIXER_WAVPACK OFF CACHE BOOL "" FORCE)
set(SDL_MIXER_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_MIXER_VENDORED ON CACHE BOOL "" FORCE)
add_subdirectory(lib/SDL_mixer EXCLUDE_FROM_ALL)

add_subdirectory(lib/entt EXCLUDE_FROM_ALL)

add_subdirectory(lib/box2d EXCLUDE_FROM_ALL)

add_subdirectory(lib/glaze EXCLUDE_FROM_ALL)

set(IMGUI_PATH "lib/imgui")
add_library(imgui STATIC EXCLUDE_FROM_ALL
    "${IMGUI_PATH}/imgui.cpp"
    "${IMGUI_PATH}/imgui_draw.cpp"
    "${IMGUI_PATH}/imgui_tables.cpp"
    "${IMGUI_PATH}/imgui_widgets.cpp"
    "${IMGUI_PATH}/backends/imgui_impl_sdl3.cpp"
    "${IMGUI_PATH}/backends/imgui_impl_sdlrenderer3.cpp"
)

target_include_directories(imgui PUBLIC
    "${IMGUI_PATH}"
    "${IMGUI_PATH}/backends"
)

target_link_libraries(imgui PRIVATE
    "SDL3::SDL3-static"
)

add_executable(${PROJECT_NAME} WIN32
    "src/main.cpp"
    "src/scenes.cpp"
    "src/systems.cpp"
    "src/presets.cpp"
    "src/factories.cpp"
    "src/levels.cpp"
    "src/settings.cpp"
    "src/assets.cpp"
    "src/interface.cpp"
    "src/render.cpp"
    "src/skills.cpp"
    "src/ai_behaviors.cpp"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4100>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Wno-unused-parameter>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    "SDL3::SDL3-static"
    "SDL3_image::SDL3_image-static"
    "SDL3_mixer::SDL3_mixer-static"
    "EnTT::EnTT"
    "box2d"
    "imgui"
    "glaze::glaze"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}
)
