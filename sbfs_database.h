/*
 * Author: Tony Allen (cyril0allen@gmail.com)
 *
 * This file defines the SbfsDatabase object which is an around the backing
 * database for the SBFS.
 */

#ifndef _SBFS_DATABASE_H_
#define _SBFS_DATABASE_H_

#include <string>

using namespace std;

class SbfsDatabase {
  public:
    // Constructor/Destructor.
    SbfsDatabase();
    ~SbfsDatabase();

    // Put a key-value pair into the database.
    void Put(const string& key, const string& value,
             std::function<void()> error_handler = [](){});

    // Return a value associated with a particular key.
    string Get(const string& key,
               std::function<void()> error_handler = [](){});
};

#endif // _SBFS_DATABASE_H_
