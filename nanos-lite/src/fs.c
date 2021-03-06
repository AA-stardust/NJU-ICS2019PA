#include "fs.h"
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}
size_t events_read(void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len); 
size_t fbsync_write(const void *buf, size_t offset, size_t len); 
size_t dispinfo_read(void *buf, size_t offset, size_t len); 
/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", -1, 0, invalid_read, invalid_write,0},
  {"stdout", -1, 0, invalid_read, serial_write,0},
  {"stderr", -1, 0, invalid_read, serial_write,0}, 
  {"/dev/fb",0,0,invalid_read,fb_write,0},
  {"/dev/fbsync",0,0,invalid_read,fbsync_write,0},
  {"/proc/dispinfo",128,0,dispinfo_read,invalid_write,0},
  {"/dev/events",0,0,events_read,invalid_write,0},
  {"/dev/tty",-1,0,invalid_read,serial_write,0},
 
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  assert(!strcmp(file_table[3].name,"/dev/fb"));
  int screen_height();
  int screen_width();
  file_table[3].size=screen_height()*screen_width()*4;
}

size_t fs_open(const char* pathname,int flags,int mode){
  //printf("%s\n", pathname);
  int i=0;
  for(i=0;i<NR_FILES;++i){
    if(!strcmp(file_table[i].name,pathname)){
      break;
    }
  }
  assert(i<NR_FILES);
  //printf("open\n");
  file_table[i].open_offset=0;
  return i;
}

size_t fs_read(int fd,void *buf,size_t len){
  //if(len==0)printf("fs_read: 000000000\n");
  //printf("%d %d\n",fd,len);
  ReadFn read = file_table[fd].read == NULL ? (ReadFn)ramdisk_read : file_table[fd].read;
  //if(read==events_read)printf("hit\n");
    if (file_table[fd].open_offset + len > file_table[fd].size) {
        len = file_table[fd].size- file_table[fd].open_offset;
    }
    int ret = read(buf, file_table[fd].open_offset + file_table[fd].disk_offset, len);
    file_table[fd].open_offset += len;
    return ret;
}
size_t fs_filesz(int fd){
  return file_table[fd].size;
}
size_t fs_close(int fd){
  return 0;
}
size_t fs_write(int fd,const void *buf,size_t len){
  //_yield();
  WriteFn write = file_table[fd].write == NULL ? (WriteFn) ramdisk_write : file_table[fd].write;
  if (file_table[fd].open_offset + len > file_table[fd].size) {
    len = file_table[fd].size - file_table[fd].open_offset;
  }
  int ret = write(buf, file_table[fd].open_offset + file_table[fd].disk_offset, len);
  file_table[fd].open_offset += len;
  return ret;
}
size_t fs_lseek(int fd,size_t offset,int whence){
  size_t old_offset=file_table[fd].open_offset;
  switch(whence){
      case SEEK_SET:
          file_table[fd].open_offset=offset;
          break;
      case SEEK_CUR:
          file_table[fd].open_offset+=offset;
          break;
      case SEEK_END:
          file_table[fd].open_offset=fs_filesz(fd)-offset;
          break;
      default:
          assert(0);
    }
  if(file_table[fd].open_offset>file_table[fd].size){
    file_table[fd].open_offset=old_offset;
    return -1;
  }
  return file_table[fd].open_offset;
}
size_t fs_openset(int fd){
  return file_table[fd].open_offset;
}
size_t fs_diskset(int fd){
  return file_table[fd].disk_offset;
}
size_t fs_read2(int fd,void *buf,size_t len){
  ReadFn read = ramdisk_read;
    if (file_table[fd].open_offset + len > file_table[fd].size) {
        len = file_table[fd].size- file_table[fd].open_offset;
    }
    int ret = read(buf, file_table[fd].open_offset + file_table[fd].disk_offset, len);
    file_table[fd].open_offset += len;
    return ret;
}