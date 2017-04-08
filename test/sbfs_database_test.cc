/*
 * Author: Tony Allen (cyril0allen@gmail.com)
 */

#include <iostream>
#include <string>
#include <time.h>

#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"

#include "../file_metadata_generated.h"
#include "../sbfs_database.h"

using namespace std;

const std::function<void()> fail_out = []() { assert(false); };

const string test_db_prefix = "/tmp/sbfs_unit_test_";

const string now = string(to_string(std::time(0)));

//-----------------------------------------------------------------------------

void test_insertion() {
  cout << "Running " << __func__ << endl;
  SbfsDatabase sbdb(test_db_prefix + __func__ + now);
  const string key = __func__;
  const string val = "test_val";
  sbdb.Put(key, val, fail_out);
  assert(sbdb.Get(key) == val);
}

//-----------------------------------------------------------------------------

void test_removal() {
  cout << "Running " << __func__ << endl;
  SbfsDatabase sbdb(test_db_prefix + __func__ + now);
  const string key = __func__;
  const string val = "test_val";
  sbdb.Put(key, val, fail_out);
  sbdb.Delete(key, fail_out);

  // We expect a failed Get() call, so the test will fail if this is false;
  bool failed_get = false;
  const auto set_failed = [&failed_get]() { failed_get = true; };
  sbdb.Get(key, set_failed);

  assert(failed_get);
}

//-----------------------------------------------------------------------------

int main(int argc, char** argv) {
  cout << "=== Running SBFS database tests === " << endl;

  test_insertion();
  test_removal();

  cout << "ALL TESTS PASSED" << endl;
  return 0;
}
