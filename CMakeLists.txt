cmake_minimum_required(VERSION 3.15)
cmake_policy(VERSION 3.15)

project(MCBubble VERSION 0.1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(PSDK_EXAMPLES OFF)
set(PSDK_INSTALL  OFF)
set(PSDK_GTASA    ON)

include(FetchContent)

FetchContent_Declare(
	kthook
	GIT_REPOSITORY https://github.com/kin4stat/kthook.git
	GIT_TAG 910feafb338e0605922c2aaa38dd471eabef4264
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/kthook
)

FetchContent_Declare(
    plugin-sdk
    GIT_REPOSITORY https://github.com/imring/plugin-sdk.git
    GIT_TAG 81b18b744b64db3058f48344c144591c9030205b
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pluginsdk
)

FetchContent_Declare(
    SAMPAPI
    GIT_REPOSITORY https://github.com/BlastHackNet/SAMP-API.git
    GIT_TAG 6d4db99ab41f19d1a6a7c6cd48f5878bd1e14b62
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sampapi
)

FetchContent_MakeAvailable(kthook plugin-sdk sampapi)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/kthook)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/kthook/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/kthook/ktsignal/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/kthook/xbyak)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME} PRIVATE
    ${${PROJECT_NAME}_LIST}
    src/main.cpp
    src/utils.cpp
 )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    SUFFIX ".asi"
)

target_link_libraries(${PROJECT_NAME} PUBLIC kthook pluginsdk::gtasa sampapi)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
