/*
 * Author: Tony Allen (cyril0allen@gmail.com)
 *
 */

#include <iostream>
#include <string>
#include <memory>

#include "sbfs_database.h"
#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"
#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"

#include "sbfs_database.h"

using namespace std;
using namespace rocksdb;

//-----------------------------------------------------------------------------

SbfsDatabase::SbfsDatabase(const string& db_path) {
  Options options;

  // Optimize the RocksDB.
  options.IncreaseParallelism();
  options.OptimizeLevelStyleCompaction();
  options.create_if_missing = true;

  // Open the DB.
  Status status = DB::Open(options, db_path, &db_);
  assert(status.ok());
}

//-----------------------------------------------------------------------------

SbfsDatabase::~SbfsDatabase() {
}

//-----------------------------------------------------------------------------

void SbfsDatabase::Put(const string& key, const string& value,
                       function<void()> error_handler) {
  assert(db_);

  Status status = db_->Put(WriteOptions(), key, value);
  if (!status.ok()) {
    error_handler();
  }
}

//-----------------------------------------------------------------------------

string SbfsDatabase::Get(const string& key, function<void()> error_handler) {
  assert(db_);

  string value;
  Status status = db_->Get(ReadOptions(), key, &value);
  if (!status.ok()) {
    error_handler();
  }
  return value;
}

//-----------------------------------------------------------------------------

void SbfsDatabase::Delete(const string& key, function<void()> error_handler) {
  assert(db_);
  Status status = db_->Delete(WriteOptions(), key);
  if (!status.ok()) {
    error_handler();
  }
}

//-----------------------------------------------------------------------------

