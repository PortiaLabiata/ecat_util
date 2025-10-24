add_library(ecat core.cpp)
target_include_directories(ecat PUBLIC "lib/SOEM/osal/linux")
target_link_libraries(ecat PUBLIC soem)
