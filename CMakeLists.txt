#
# Author: Tony Allen (cyril0allen@gmail.com)
#

cmake_minimum_required(VERSION 3.7)
project(sbfs)

#SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#SET(BUILD_SHARED_LIBRARIES OFF)
#SET(CMAKE_EXE_LINKER_FLAGS "-static")
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O2 --std=c++14")

# Create the flatbuffer.
execute_process(COMMAND flatc --cpp file_metadata.fbs)

add_library(SbfsDB sbfs_database.cc)
add_library(rocksdb STATIC IMPORTED)
add_library(flatbuffers STATIC IMPORTED)

#find_package(RocksDB)

target_link_libraries(SbfsDB rocksdb)

#target_include_directories(Sbfs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(sbfs sbfs.cc sbfs_database.cc )
