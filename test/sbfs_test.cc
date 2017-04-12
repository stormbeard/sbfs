/*
 * Author: Tony Allen (cyril0allen@gmail.com)
 *
 */

#include <iostream>
#include <string>
#include <time.h>

#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"

#include "../file_metadata_generated.h"
#include "../sbfs.h"
#include "../sbfs_database.h"

using namespace std;

const std::function<void()> fail_out = []() { assert(false); };

const string test_db_prefix = "/tmp/sbfs_unit_test_";

const string now = string(to_string(std::time(0)));

//-----------------------------------------------------------------------------

int64_t next_unique_val() {
  static int64_t val = 0;
  return ++val;
}

//-----------------------------------------------------------------------------

void test_getattr() {
  cout << "Running " << __func__ << endl;
  const string db_location = test_db_prefix + __func__ + now;
  SbfsTest sbfs(db_location);
  const string filename = "/some/test/filename";

  flatbuffers::FlatBufferBuilder builder;
  const int64_t size_bytes = next_unique_val();
  const int64_t user_id = next_unique_val();
  const int64_t group_id = next_unique_val();
  const int64_t mode = next_unique_val();
  const uint64_t link_count = next_unique_val();
  const int64_t last_access_time= next_unique_val();
  const int64_t last_file_modified_time = next_unique_val();
  const int64_t last_inode_modified_time = next_unique_val();
  const sbfs::FileType file_type = sbfs::FileType_File;
  const vector<int64_t> bogus_block_offsets({1,2,3});
  const auto block_vec = builder.CreateVector(bogus_block_offsets);

  auto fmloc = CreateFileMetadata(builder,
                                  size_bytes,
                                  user_id,
                                  group_id,
                                  mode,
                                  link_count,
                                  last_access_time,
                                  last_file_modified_time,
                                  last_inode_modified_time,
                                  file_type,
                                  block_vec);

  builder.Finish(fmloc);
  const char *data = reinterpret_cast<char *>(builder.GetBufferPointer());
  const auto size = builder.GetSize();
  sbfs.db().Put(filename, string(data, size), fail_out);

  // Verify GetAttr works.
  struct stat st_buf;
  sbfs.GetAttr(filename.data(), &st_buf);

  assert(st_buf.st_size == size_bytes);
  assert(st_buf.st_uid == user_id);
  assert(st_buf.st_gid == group_id);
  assert(st_buf.st_mode == mode);
  assert(st_buf.st_nlink == link_count);
  assert(st_buf.st_atime == last_access_time);
  assert(st_buf.st_mtime == last_file_modified_time);
  assert(st_buf.st_ctime == last_inode_modified_time);
  assert(static_cast<uint64_t>(st_buf.st_blocks) ==
         bogus_block_offsets.size());
}

//-----------------------------------------------------------------------------

int main(int argc, char** argv) {
  cout << "=== Running SBFS tests === " << endl;

  test_getattr();

  cout << "ALL TESTS PASSED" << endl;
  return 0;
}

