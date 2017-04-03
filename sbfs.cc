/*
 * Author: Tony Allen (cyril0allen@gmail.com)
 *
 */

#include <iostream>
#include <string>
#include <memory>

//#include "flatbuffers/idl.h"
//#include "flatbuffers/util.h"
#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"

#include "sbfs.h"

using namespace std;
using namespace rocksdb;

//-----------------------------------------------------------------------------

const string kDBPath = "/tmp/rocksdb_simple_example";

// TODO: Don't hardcode this.
static constexpr int64_t kSbfsBlockSize = 8 * 1024;

//-----------------------------------------------------------------------------

int main(int argc, char** argv) {
  DB *db;
  Options options;

  // Optimize RocksDB. This is the easiest way to get RocksDB to perform well.
  options.IncreaseParallelism();
  options.OptimizeLevelStyleCompaction();
  // create the DB if it's not already present
  options.create_if_missing = true;

  // open DB
  Status s = DB::Open(options, kDBPath, &db);
  assert(s.ok());

  // Put key-value
  s = db->Put(WriteOptions(), "key1", "value");
  assert(s.ok());
  std::string value;
  // get value
  s = db->Get(ReadOptions(), "key1", &value);
  assert(s.ok());
  assert(value == "value");

  // atomically apply a set of updates
  {
    WriteBatch batch;
    batch.Delete("key1");
    batch.Put("key2", value);
    s = db->Write(WriteOptions(), &batch);
  }

  s = db->Get(ReadOptions(), "key1", &value);
  assert(s.IsNotFound());

  db->Get(ReadOptions(), "key2", &value);
  assert(value == "value");

  return 0;
}

//-----------------------------------------------------------------------------

