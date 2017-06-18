/*
 * Author: Tony Allen (cyril0allen@gmail.com)
 *
 */

#include <fuse.h>
#include <iostream>
#include <string>
#include <time.h>

#include "errno.h"
#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"

#include "../file_metadata_generated.h"
#include "../sbfs.h"
#include "../sbfs_database.h"

using namespace std;

const std::function<void()> fail_out = []() { assert(false); };

//-----------------------------------------------------------------------------

class SbfsTest : public Sbfs {
 public:
  SbfsTest(const std::string& db_location) : Sbfs(db_location) {}

  bool TestIsValidFd(int fd) { return IsValidFd(fd); }

  void dump(){
    cout << "@tallen: " << fd_map_.size() << endl;
  }

  // Accessors.
  SbfsDatabase& db() { return db_; }
};

//-----------------------------------------------------------------------------

class Tester {
 public:
  // Constructor/destructor.
  Tester();
  ~Tester() = default;

  void TestGetAttr();
  void TestOpen();

 private:
  // Return an integer different from any other integers returned from this
  // function previously.
  int64_t NextUniqueVal() {
    return ++unique_val_;
  }

  // Creates a database location given some identifier string.
  string CreateDbLocation(const string& identifier) {
    const string test_db_prefix = "/tmp/sbfs_unit_test_";
    const string now = string(to_string(std::time(0)));
    return test_db_prefix + identifier + now;
  }

 private:
  // Variable holding the last unique integer returned from 'NextUniqueVal'.
  int64_t unique_val_;
};

//-----------------------------------------------------------------------------

Tester::Tester() :
  unique_val_(0) {

}

//-----------------------------------------------------------------------------

void Tester::TestGetAttr() {
  cout << "Running " << __func__ << endl;
  const string db_location = CreateDbLocation(__func__);
  SbfsTest sbfs(db_location);
  const string filename = "/some/test/filename";

  flatbuffers::FlatBufferBuilder builder;
  const int64_t size_bytes = NextUniqueVal();
  const int64_t user_id = NextUniqueVal();
  const int64_t group_id = NextUniqueVal();
  const int64_t mode = NextUniqueVal();
  const uint64_t link_count = NextUniqueVal();
  const int64_t last_access_time= NextUniqueVal();
  const int64_t last_file_modified_time = NextUniqueVal();
  const int64_t last_inode_modified_time = NextUniqueVal();
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

void Tester::TestOpen() {
  cout << "Running " << __func__ << endl;
  const string db_location = CreateDbLocation(__func__);
  SbfsTest sbfs(db_location);
  const string filepath = "/some/test/filename";

  unique_ptr<struct fuse_file_info> fuse_file_info =
    make_unique<struct fuse_file_info>();
  fuse_file_info->flags = 0;

  // Make sure we report bogus fd.
  assert(!sbfs.TestIsValidFd(1337));

  // Make sure we can't open a file that hasn't been created yet.
  const int fd = sbfs.Open(filepath.c_str(), fuse_file_info.get());
  assert(fd == -ENODATA);

  // TODO: create a file and test it can be opened.
}

//-----------------------------------------------------------------------------

int main(int argc, char** argv) {
  cout << "=== Running SBFS tests === " << endl;
  Tester tester;
  tester.TestGetAttr();
  tester.TestOpen();
  cout << "ALL TESTS PASSED" << endl;
  return 0;
}

