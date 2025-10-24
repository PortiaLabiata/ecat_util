cmake_minimum_required(VERSION 3.12)
project(ecat_util LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
	-Werror
	-Wall
	-Wextra
	-lm
)

add_executable(main main.cpp window.cpp)
include_directories("lib/imgui/")
include_directories("lib/imgui/backends")
include_directories("lib/SOEM/include")
add_subdirectory("lib/SOEM")
add_subdirectory("lib/imgui")
add_subdirectory("slave")

target_link_libraries(main PUBLIC soem)
target_link_libraries(main PUBLIC ImGui)
target_link_libraries(main PUBLIC slave)
