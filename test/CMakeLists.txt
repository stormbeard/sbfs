#
# Author: Tony Allen (cyril0allen@gmail.com)
#

add_executable(sbfs_database_test sbfs_database_test.cc)
target_link_libraries(sbfs_database_test LINK_PUBLIC SbfsDB rocksdb)
