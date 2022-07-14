cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(MARTY_All_Particles_IO)

find_library(marty
        NAMES marty libmarty
        HINTS /usr/local/lib)
#target_link_libraries(marty -lmarty)
#target_link_libraries(marty)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(QED_AllParticles_IO.x QED_AllParticles_IO.cpp)
target_link_libraries(QED_ALlParticles_IO.x marty)
#find_library(RESTBED_LIBRARY
#        NAMES
#        restbed
#        PATHS
#        ${RESTBED_ROOT}/build_release
#        ${RESTBED_ROOT}/Release
#        )
