/*
 * Author: Tony Allen (cyril0allen@gmail.com)
 *
 */

#include <iostream>
#include <string>
#include <memory>

#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"

#include "file_metadata_generated.h"
#include "sbfs.h"
#include "sbfs_database.h"

using namespace flatbuffers;
using namespace sbfs;
using namespace std;

//-----------------------------------------------------------------------------

const string kDBPath = "/tmp/rocksdb_simple_example";

// TODO: Don't hardcode this.
//static constexpr int64_t kSbfsBlockSize = 8 * 1024;

//-----------------------------------------------------------------------------

int main(int argc, char** argv) {
  // Create DB.
  SbfsDatabase sbdb(kDBPath);

  // Build a metadata flatbuffer.
  flatbuffers::FlatBufferBuilder builder;
  auto fmloc = CreateFileMetadata(builder, 1,2,3,4,5,6,7,8, FileType_File);
  builder.Finish(fmloc);

  const char *data = reinterpret_cast<char *>(builder.GetBufferPointer());
  const int64_t size = builder.GetSize();

  const string inode_str(data, size);

  sbdb.Put("tony", inode_str);
  const string what = sbdb.Get("tony");
  uint8_t *buffer_ptr =
    reinterpret_cast<uint8_t *>(const_cast<char *>(what.data()));
  auto metadata = GetFileMetadata(buffer_ptr);

  assert(metadata->size_bytes() == 1);
  assert(metadata->user_id() == 2);
  assert(metadata->last_inode_modified_time() == 8);

  cout << "all good" << endl;

  return 0;
}

//-----------------------------------------------------------------------------

