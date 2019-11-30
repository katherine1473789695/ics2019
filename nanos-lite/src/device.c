#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for(int i=0;i<len;i++){
    _putc(((char *)buf)[i]);
  }
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  //sprintf(buf,"t %d\n",uptime());
  Log("here");
  uint32_t time = uptime();
  Log("time: %d",time);
  sprintf(buf,"t %d\n",time);
  int key = read_key();
  bool down =false;
  if(key & 0x8000){
    key ^=0x8000;
    down = true;
  }
  if(key == _KEY_NONE){
    //uint32_t time = uptime();
    sprintf(buf,"t %d\n",uptime());
  }else{
    sprintf(buf,"%s %s\n",down ? "kd":"ku",keyname[key]);
  }

  return strlen(buf);
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  memcpy(buf, &dispinfo[offset], len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  //Log("fb_write\n");
  int x,y;
  int len1,len2,len3;
  offset = offset>>2;
  y = offset/ screen_width();
  x = offset% screen_width();
  len = len>>2;
  len1=len2=len3=0;

  len1 = len<= screen_width()-x ? len : screen_width()-x;
  draw_rect((uint32_t*)buf,x,y,len1,1);

  if((len>len1)&&((len-len1)>screen_width())){
    len2 = len-len1;
    draw_rect((uint32_t*)buf+len1,0,y+1,screen_width(),len2/screen_width());
  }

  if(len-len1-len2>0){
    len3 = len-len1-len2;
    draw_rect((uint32_t*)buf+len1+len2,0,y+len2/screen_width()+1,len3,1);
  }
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
}
