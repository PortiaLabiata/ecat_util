add_library(slave core.cpp)
target_include_directories(slave PUBLIC "lib/SOEM/osal/linux")
target_link_libraries(slave PUBLIC soem)
