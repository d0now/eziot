cmake_minimum_required(VERSION 3.20.2)
project(eziot-service)

# Check build type.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)

set(BUILD_TARGET "armv5l-linux-gnueabi")

################################################################################
# pre-build check.
################################################################################

# Check clang.
if(NOT CMAKE_C_COMPILER MATCHES "clang$")
    message(FATAL_ERROR "Please use clang by CMAKE_C_COMPILER flag.")
endif()
if(NOT CMAKE_ASM_COMPILER MATCHES "clang$")
    message(FATAL_ERROR "Please use clang by CMAKE_ASM_COMPILER flag.")
endif()

################################################################################
# Build options.
################################################################################

add_executable(${CMAKE_PROJECT_NAME})

target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        src/main.c
        src/proc.c
        src/peer.c
)

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
        /usr/arm-linux-gnueabi/include/
)

# Add compile options.
target_compile_options(${CMAKE_PROJECT_NAME}
    PUBLIC
        --target=${BUILD_TARGET} -no-pie
)

# Add link options.
target_link_options(${CMAKE_PROJECT_NAME}
    PUBLIC
        --target=${BUILD_TARGET} -no-pie -static
)

# Add libraries.
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE pthread)