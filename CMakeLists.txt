#
# Author: Tony Allen (cyril0allen@gmail.com)
#

cmake_minimum_required(VERSION 3.7)
project(SBFS) 

set(CMAKE_CXX_FLAGS "-Wall -std=c++14 -g -D_FILE_OFFSET_BITS=64")

# Find RocksDB.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
find_package(Rocksdb)
include_directories(${ROCKSDB_INCLUDE_DIR})

# Create the flatbuffer.
execute_process(COMMAND flatc --cpp file_metadata.fbs)

add_subdirectory(test)

add_library(SbfsDB SHARED sbfs_database.cc)
target_include_directories(SbfsDB PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(SbfsLib SHARED sbfs.cc)
target_include_directories(SbfsLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(sbfs sbfs_main.cc sbfs.cc)

target_link_libraries(sbfs LINK_PUBLIC SbfsDB rocksdb)

# Unit testing
enable_testing()
add_test(sbfs_test test/sbfs_test)
add_test(sbfs_database_test test/sbfs_database_test)
