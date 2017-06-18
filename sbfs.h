/*
 * Author: Tony Allen (cyril0allen@gmail.com)
 *
 * SBFS
 *
 */

#ifndef _SBFS_H_
#define _SBFS_H_

#include <fuse.h>
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
  //
  // Unsupported operations:
  //   -- Permissions.
  //   -- FD_CLOEXEC, O_DSYNC, O_EXCL, O_NOCTTY, O_NONBLOCK, O_RSYNC, O_SYNC,
  //      O_TRUNC, O_TTY_INIT
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

    int OpenDir() {
      // TODO: Implement.
      assert(false);
      return -1;
    }

    int ReadDir() {
      // TODO: Implement.
      assert(false);
      return -1;
    }

    int MkDir() {
      // TODO: Implement.
      assert(false);
      return -1;
    }

    int RmDir() {
      // TODO: Implement.
      assert(false);
      return -1;
    }

 protected:
  static const bool IsFieldSet(int flag, int field) { return (flag & field); }

  // Verify if file descriptor is valid;
  bool IsValidFd(int fd) const { return fd_map_.count(fd) > 0; }

  // Deserialize string to a metadata flatbuffer.
  const sbfs::FileMetadata *DeserializeToMetadata(
    const std::string& metadata_string);

  // Returns an unused file descriptor ID.
  int GetAvailableFd();

  // Verify the correctness of Open() flags.
  static const bool OpenFlagsOk(int flags);

 protected:
  // Path to the database file.
  const std::string db_path_;

  // The interface to the filesystem's backing database.
  SbfsDatabase db_;

  // The last allocated file descriptor.
  int current_allocated_fd_;

  // Container containing previously allocated file descriptors that are now
  // free.
  std::vector<int> free_fds_;

  // Data associated with an open file.
  typedef struct OpenFileInfo {
    // File descriptor associated with this open file.
    int fd;

    // File offest marking the current position within the file.
    int64_t position_offset;

    // Special treatment of file after opening.
    bool read_allowed : 1;
    bool write_allowed : 1;
    bool execute_allowed : 1;

    // Whether to set the offset to the end of the file after each write.
    bool set_offset_to_end : 1;
  } OpenFileInfo;

  // Mapping from file descriptor to the filename and inode ID.
  std::unordered_map<int, OpenFileInfo> fd_map_;
};

//-----------------------------------------------------------------------------

#endif // _SBFS_H_
