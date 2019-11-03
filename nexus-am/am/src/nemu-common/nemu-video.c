#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include<string.h>
#define W 400
#define H 300
static uint32_t fb[W * H] = {};
static inline int min(int x, int y) {
  return (x < y) ? x : y;
}
size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      info->width = W;
      info->height = H;
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}
size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
      int x=ctl->x,y=ctl->y,w=ctl->w,h=ctl->h;
      uint32_t *pixels=ctl->pixels;
      int cp_bytes=sizeof(uint32_t) * min(w, W - x);
      for (int j = 0; j < h && y + j < H; j ++) {
        memcpy(&fb[(y + j) * W + x], pixels, cp_bytes);
        pixels += w;
      }
      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      }
      return sizeof(_DEV_VIDEO_FBCTL_t);
    }
  }
  return 0;
}
extern int screen_width();
extern int screen_height();
extern void draw_sync();
void __am_vga_init() {
  int i;
  int size=screen_width()*screen_height();
  uint32_t *fb=(uint32_t *)(uintptr_t )FB_ADDR;
  for(i=0;i<size;i++)fb[i]=i;
  draw_sync();
}
