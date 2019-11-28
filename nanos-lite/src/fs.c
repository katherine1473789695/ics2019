#include "fs.h"




enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}


int fs_open(const char *pathname, int flags, int mode){
  for(int i=0;i<NR_FILES;i++){
    if(strcmp(file_table[i].name,pathname)==0){
      file_table[i].open_offset=0;
      return i;
    }
  }
  printf("file %s does not exist!",pathname);
  assert(0);
  return -1;
}

size_t fs_read(int fd,void *buf,size_t len){
  size_t read = -1;
  switch(fd){
    case FD_STDIN:
    case FD_STDOUT:
    case FD_STDERR:read=0;break;
    default:{
      assert(0<=fd && fd<NR_FILES);
      Finfo *f=&file_table[fd];
      read = (f->open_offset+len>f->size) ? (f->size-f->open_offset):len;
      ramdisk_read(buf,f->disk_offset+f->open_offset,read);
      f->open_offset+=read;
      break;
    }
  }

  return read;
}

int fs_close(int fd){
  return 0;
}