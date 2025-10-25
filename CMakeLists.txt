cmake_minimum_required(VERSION 3.12)
project(ecat_util LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_PATH lib)
set(SUBMODULES panels common)

add_compile_options(
	-Werror
	-Wall
	-Wextra
	-lm
)

add_executable(main main.cpp window.cpp)
include_directories(${LIB_PATH}/imgui)
include_directories(${LIB_PATH}/imgui/backends)
include_directories(${LIB_PATH}/SOEM/include)

add_subdirectory(${LIB_PATH}/SOEM)
add_subdirectory(${LIB_PATH}/imgui)
add_subdirectory(ecat)
add_subdirectory(panels)
target_include_directories(ecat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(main PUBLIC ecat)
target_link_libraries(main PUBLIC soem)
target_link_libraries(main PUBLIC ImGui)
target_link_libraries(main PUBLIC panels)
