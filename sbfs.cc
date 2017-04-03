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
//static constexpr int64_t kSbfsBlockSize = 8 * 1024;

//-----------------------------------------------------------------------------

int main(int argc, char** argv) {
  return 0;
}

//-----------------------------------------------------------------------------

