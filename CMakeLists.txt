cmake_minimum_required(VERSION 3.0.0)
project(nes-emulator VERSION 0.1.0)

include(CTest)
enable_testing()

SET(GCC_COVERAGE_COMPILE_FLAGS "-std=c++17")
SET(GCC_COVERAGE_LINK_FLAGS    "-lX11 -lGL -lpthread -lpng -lstdc++fs")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

file(GLOB source_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_subdirectory(mappers)

add_executable(nes-emulator "${source_files}")

target_link_libraries(nes-emulator mappers)

# target_include_directories(nes-emulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
