/*
 * Author: Tony Allen (cyril0allen@gmail.com)
 *
 * This file defines the SbfsDatabase object which is an around the backing
 * database for the SBFS.
 */

#ifndef _SBFS_DATABASE_H_
#define _SBFS_DATABASE_H_

#include <string>

#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"
#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"

class SbfsDatabase {
 public:
  // Constructor/Destructor.
  explicit SbfsDatabase(const std::string& db_path);
  ~SbfsDatabase();

  // Put a key-value pair into the database.
  void Put(const std::string& key, const std::string& value,
           std::function<void()> error_handler = [](){});

  // Return a value associated with a particular key.
  std::string Get(const std::string& key,
             std::function<void()> error_handler = [](){});

 private:
  // Pointer to the RocksDB instance this class wraps.
  rocksdb::DB *db_;
};

#endif // _SBFS_DATABASE_H_
