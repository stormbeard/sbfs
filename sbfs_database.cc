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

using namespace std;
using namespace rocksdb;

//-----------------------------------------------------------------------------

SbfsDatabase::SbfsDatabase() {

}

//-----------------------------------------------------------------------------

SbfsDatabase::~SbfsDatabase() {
}

//-----------------------------------------------------------------------------

