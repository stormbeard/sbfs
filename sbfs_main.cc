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

//-----------------------------------------------------------------------------

int main(int argc, char** argv) {
  Sbfs sbfs(kDBPath);

  cout << "all good" << endl;

  return 0;
}

//-----------------------------------------------------------------------------

