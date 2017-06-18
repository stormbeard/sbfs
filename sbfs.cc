/*
 * Author: Tony Allen (cyril0allen@gmail.com)
 *
 */

#include <fuse.h>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <fcntl.h>

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
                                         db_(db_path_),
                                         current_allocated_fd_(2) {
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

int Sbfs::GetAvailableFd() {
  assert(current_allocated_fd_ >= 0);

  // If there's an fd that was used and released, just return that one.
  if (free_fds_.size() > 0) {
    const int fd = free_fds_.back();
    free_fds_.pop_back();
    return fd;
  }

  if (current_allocated_fd_ < std::numeric_limits<int>::max()) {
    return ++current_allocated_fd_;
  }

  // If the next fd would cause an overflow, we're out of available file
  // descriptor IDs.
  errno = EMFILE;
  return -EMFILE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Fuse operations.

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int Sbfs::GetAttr(const char *path, struct stat *stat_buffer) {
  assert(path);
  assert(stat_buffer);

  int err = 0;
  const auto set_err = [&err]() { err = -1; };
  const string serialized_metadata = db_.Get(string(path), set_err);
  if (err != 0) {
    // This can only really mean it's not in the database.
    errno = ENODATA;
    return -ENODATA;
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

  return 0;
}

//-----------------------------------------------------------------------------
int Sbfs::MkNod(const char *path, mode_t mode, dev_t dev) {
  // Man page for mknod states the only portable use of mknod is to create a
  // FIFO-special file. We assert this for simplicity in this project.
  assert(mode == S_IFIFO);

  //const long file_type = mode & ~S_IFMT;

  // TODO: Currently ignoring permissions.
  FlatBufferBuilder fb_builder;
  const vector<int64_t> offsets;
  auto fmloc = CreateFileMetadata(fb_builder,
                                  0 /* size_bytes */,
                                  0 /* user_id */,
                                  0 /* group_id */,
                                  mode,
                                  1 /* link_count */,
                                  std::time(0) /* last_access_time */,
                                  std::time(0) /* last_file_modified_time */,
                                  std::time(0) /* last_inode_modified_time */,
                                  // TODO: fix this hard-coded file type.
                                  FileType_File,
                                  fb_builder.CreateVector(offsets));
  // TODO: make easier interface to sbfs db.
  fb_builder.Finish(fmloc);
  const char *data = reinterpret_cast<char *>(fb_builder.GetBufferPointer());
  const auto size = fb_builder.GetSize();
  db_.Put(path, string(data, size));

  return 0;
}

//-----------------------------------------------------------------------------

int Sbfs::Open(const char *path, struct fuse_file_info *fuse_fi) {
  assert(path);
  assert(fuse_fi);

  const int flags = fuse_fi->flags;

  // According to fuse docs, no creation flags will be passed to this function.
  assert(!IsFieldSet(flags, O_CREAT));
  assert(!IsFieldSet(flags, O_EXCL));

  // TODO: What happens on double-open?
  //
  // TODO: Permissions

  int err = 0;
  const auto set_err = [&err]() { err = -1; };
  const string serialized_metadata = db_.Get(string(path), set_err);
  if (err != 0) {
    // This can only really mean it's not in the database.
    errno = ENODATA;
    return -ENODATA;
  }

  const FileMetadata *metadata = DeserializeToMetadata(serialized_metadata);


  // Check for unsupported operations.
  if (metadata == nullptr) {
    // Cannot create files, so if it doesn't exist fail out since fuse should
    // have caught this.
    assert(false);
  }

  if (IsFieldSet(flags, O_DIRECTORY) &&
      metadata->type() == FileType_Directory) {
    errno = ENOTDIR;
    return -ENOTDIR;
  }

  if (IsFieldSet(flags, O_NOFOLLOW) && metadata->type() == FileType_Symlink) {
    errno = ELOOP;
    return -ELOOP;
  }

  const int fd = GetAvailableFd();
  OpenFileInfo open_fi = {fd,     /* fd */
                          0,      /* position_offset */
                          true,   /* read_allowed */
                          true,   /* write_allowed */
                          true,   /* execute_allowed */
                          false,  /* set_offset_to_end */};

  // Set any special modifiers on the open file.
  if (IsFieldSet(flags, O_RDONLY)) {
    open_fi.write_allowed = false;
    open_fi.execute_allowed = false;
  } else if (IsFieldSet(flags, O_RDWR)) {
    open_fi.execute_allowed = false;
  } else if (IsFieldSet(flags, O_WRONLY)) {
    open_fi.execute_allowed = false;
    open_fi.read_allowed = false;
  }
  if (IsFieldSet(flags, O_APPEND)) {
    open_fi.set_offset_to_end = true;
  }

  fd_map_.emplace(fd, open_fi);
  return fd;
}

//-----------------------------------------------------------------------------

bool const Sbfs::OpenFlagsOk(int flags) {
  // Exactly one of O_RDONLY, O_RDWR, O_WRONLY should be set.
  // See which of these are set and unset a single bit. If the result is 0,
  // then only one was set.
  const int flags_set = ((flags & O_RDONLY) |
                         (flags & O_RDWR) |
                         (flags & O_WRONLY));
  return (flags_set == 0) || ((flags_set & (flags_set - 1)) == 0);
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

