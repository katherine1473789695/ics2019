#include <am.h>
#include <amdev.h>
#include <nemu.h>

void draw_sync();
int screen_width();
int screen_height();

size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      //uint32_t data=inl(SCREEN_ADDR);
      info->width = 400;//data>>16;
      info->height = 300;//data & 0xffff;
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
      int width = screen_width();
      int x=ctl->x,y=ctl->y,h=ctl->h,w=ctl->w;
      uint32_t *pixels = ctl->pixels;
      uint32_t *fb=(uint32_t *)(uintptr_t)FB_ADDR;
      for(int i=0;i<h;i++){
        for(int j=0;j<w;j++){
          fb[(y+i)*width+x]=pixels[i];
        }
      }

      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      }
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
  int i;
  int size = screen_width()*screen_height();
  uint32_t *fb=(uint32_t *)(uintptr_t)FB_ADDR;
  for(i=0;i<size;i++)fb[i]=i;
  draw_sync();
}
