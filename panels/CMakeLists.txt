add_library(panels OBJECT panel_slaves.cpp panel_console.cpp panel_welcome.cpp panel.cpp)
target_include_directories(panels PUBLIC ecat)
target_link_libraries(panels PUBLIC ecat)
