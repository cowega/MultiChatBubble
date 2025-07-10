cmake_minimum_required(VERSION 3.15)
cmake_policy(VERSION 3.15)

project(MultiChatBubble VERSION 1.2.0)

option(BUILD_SAMP_R1 "Build for SAMP R1 version" OFF)
option(BUILD_SAMP_R3 "Build for SAMP R3 version" OFF)

if (NOT BUILD_SAMP_R1 AND NOT BUILD_SAMP_R3)
	message(FATAL_ERROR "Choose a target SAMP version. Example: -DBUILD_SAMP_R1=ON or -DBUILD_SAMP_R3=ON")
elseif (BUILD_SAMP_R1 AND BUILD_SAMP_R3)
	message(FATAL_ERROR "Choose only one version. Turn off one: -DBUILD_SAMP_R1=OFF or -DBUILD_SAMP_R3=OFF")
endif()

add_library(${PROJECT_NAME} SHARED)

if (BUILD_SAMP_R1)
	target_compile_definitions(${PROJECT_NAME} PRIVATE BUILD_SAMP_R1)
	set(OUT_NAME "${PROJECT_NAME}_R1")
elseif (BUILD_SAMP_R3)
	target_compile_definitions(${PROJECT_NAME} PRIVATE BUILD_SAMP_R3)
	set(OUT_NAME "${PROJECT_NAME}_R3")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

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

FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG 913151caa8905370229e361486b10c890aeaed45
)

FetchContent_Declare(
  simpleini
  GIT_REPOSITORY https://github.com/brofield/simpleini.git
  GIT_TAG master
)

FetchContent_MakeAvailable(kthook plugin-sdk sampapi imgui simpleini)

file(GLOB IMGUI_SRC
	"${imgui_SOURCE_DIR}/*.cpp"
	"${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp"
	"${imgui_SOURCE_DIR}/backends/imgui_impl_dx9.cpp"
)

add_library(imgui STATIC ${IMGUI_SRC})

target_include_directories(imgui PUBLIC
	${imgui_SOURCE_DIR}
	${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC d3d9)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/kthook)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/kthook/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/kthook/ktsignal/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/kthook/xbyak)

target_include_directories(${PROJECT_NAME} PRIVATE ${simpleini_SOURCE_DIR})

target_sources(${PROJECT_NAME} PRIVATE
	${${PROJECT_NAME}_LIST}
	src/main.cpp
	src/utils.cpp
	src/imgui_utils.cpp
	src/settings.cpp
)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	OUTPUT_NAME ${OUT_NAME}
	SUFFIX ".asi"
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
	kthook
	pluginsdk::gtasa
	sampapi
	imgui
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
