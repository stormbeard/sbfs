/*
 * Author: Tony Allen (cyril0allen@gmail.com)
 *
 * SBFS
 *
 */

#ifndef _SBFS_H_
#define _SBFS_H_

#include <string>
#include <unordered_map>

#include "sbfs_database.h"

class Sbfs {
 public:
  // Constructor.
  Sbfs(const std::string& sbfs_db_path);

  // Destructor.
  ~Sbfs() = default;

  /* Fuse operations */

  // Supported operations. As new operations are supported, be sure to update
  // the fuse operations struct.
  //
  // The function descriptions are taken from /usr/include/fuse/fuse.h.

  // Get file attributes.
  //
  // Similar to stat(). The 'st_dev' and 'st_blksize' fields are ignored.  The
  // 'st_ino' field is ignored except if the 'use_ino' mount option is given.
  int GetAttr(const char *path, struct stat *stat_buffer);

  // File open operation.
  //
  // No creation, or truncation flags (O_CREAT, O_EXCL, O_TRUNC) will be passed
  // to open().  Open should check if the operation is permitted for the given
  // flags.  Optionally open may also return an arbitrary filehandle in the
  // fuse_file_info structure, which will be passed to all file operations.
  int Open(const char *path, struct fuse_file_info *fuse_fi);

  // Read data from an open file.
  //
  // Read should return exactly the number of bytes requested except on EOF or
  // error, otherwise the rest of the data will be substituted with zeroes.  An
  // exception to this is when the 'direct_io' mount option is specified, in
  // which case the return value of the read system call will reflect the
  // return value of this operation.
  int Read(const char *path,
           char *buffer,
           size_t size,
           off_t offset,
           struct fuse_file_info *fuse_fi);

   // Write data to an open file
   // Write should return exactly the number of bytes requested except on
   // error.  An exception to this is when the 'direct_io' mount option is
   // specified (see read operation).
   int Write(const char *path,
             const char *buffer,
             size_t size,
             off_t offset,
             struct fuse_file_info *);

    int OpenDir();

    int ReadDir();

    int MkDir();

    int RmDir();

 protected:
  // Verify if file descriptor is valid;
  bool IsValidFd(int fd);

  // Deserialize string to a metadata flatbuffer.
  const sbfs::FileMetadata *DeserializeToMetadata(
    const std::string& metadata_string);

 protected:
  // Path to the database file.
  const std::string db_path_;

  // The interface to the filesystem's backing database.
  SbfsDatabase db_;

  // Mapping from file descriptor to the filename and inode ID.
  std::unordered_map<int, std::pair<std::string, int64_t>> fd_map_;
};

//-----------------------------------------------------------------------------

// Used only for unit test.
class SbfsTest : public Sbfs {
 public:
  SbfsTest(const std::string& db_location) : Sbfs(db_location) {}
  SbfsDatabase& db() { return db_; }
};

//-----------------------------------------------------------------------------

#endif // _SBFS_H_
