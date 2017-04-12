/*
 * Author: Tony Allen (cyril0allen@gmail.com)
 *
 */

#include <iostream>
#include <string>
#include <memory>

#include "errno.h"
#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"

#include "file_metadata_generated.h"
#include "sbfs.h"
#include "sbfs_database.h"

using namespace flatbuffers;
using namespace sbfs;
using namespace std;

//-----------------------------------------------------------------------------

// TODO: Don't hardcode this.
static const int32_t kDefaultBlockSize = 4 * 1024;

//-----------------------------------------------------------------------------

Sbfs::Sbfs(const string& sbfs_db_path) : db_path_(sbfs_db_path),
                                         db_(db_path_) {
}

//-----------------------------------------------------------------------------

const FileMetadata *Sbfs::DeserializeToMetadata(
  const string &metadata_string) {

  uint8_t *buffer_ptr =
    reinterpret_cast<uint8_t *>(
      const_cast<char *>(metadata_string.data()));
  return GetFileMetadata(buffer_ptr);
}

//-----------------------------------------------------------------------------

int Sbfs::GetAttr(const char *path, struct stat *stat_buffer) {
  int err = 0;
  const auto set_err = [&err]() { err = -1; };
  const string serialized_metadata = db_.Get(string(path), set_err);
  if (err != 0) {
    // This can only really mean it's not in the database.
    errno = ENODATA;
  }

  const FileMetadata *metadata = DeserializeToMetadata(serialized_metadata);

  memset(stat_buffer, 0, sizeof(struct stat));
  stat_buffer->st_mode =
    static_cast<mode_t>(metadata->mode());
  stat_buffer->st_nlink =
    static_cast<nlink_t>(metadata->link_count());
  stat_buffer->st_uid =
    static_cast<uid_t>(metadata->user_id());
  stat_buffer->st_gid =
    static_cast<gid_t>(metadata->group_id());
  stat_buffer->st_size =
    static_cast<off_t>(metadata->size_bytes());
  stat_buffer->st_atime =
    static_cast<time_t>(metadata->last_access_time());
  stat_buffer->st_mtime =
    static_cast<time_t>(metadata->last_file_modified_time());
  stat_buffer->st_ctime =
    static_cast<time_t>(metadata->last_inode_modified_time());
  stat_buffer->st_blksize =
    static_cast<blksize_t>(kDefaultBlockSize);
  const int numblk = metadata->offsets()->size();
  stat_buffer->st_blocks = static_cast<blkcnt_t>(numblk);

  return err;
}

//-----------------------------------------------------------------------------

int Sbfs::Open(const char *path, struct fuse_file_info *fuse_fi) {
  // TODO: Implement.
  assert(false);
  return -1;
}

//-----------------------------------------------------------------------------

int Sbfs::Read(const char *path,
               char *buffer,
               const size_t size,
               const off_t offset,
               struct fuse_file_info *fuse_fi) {
  // TODO: Implement.
  assert(false);
  return -1;
}

//-----------------------------------------------------------------------------

