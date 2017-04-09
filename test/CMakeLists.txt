#
# Author: Tony Allen (cyril0allen@gmail.com)
#

add_executable(sbfs_database_test sbfs_database_test.cc)
add_executable(sbfs_test sbfs_test.cc)

target_link_libraries(sbfs_database_test LINK_PUBLIC SbfsDB rocksdb)
target_link_libraries(sbfs_test LINK_PUBLIC SbfsLib SbfsDB rocksdb)
