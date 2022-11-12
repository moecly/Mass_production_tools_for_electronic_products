#include <disp_manger.h>
#include <framebuffer.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static disp_ops *r_dops;
static disp_ops *disp_dft;
static disp_buff dpbuff;
static int line_width;
static int pixel_width;

/*
 * 画点
 */
int put_pixel(int x, int y, unsigned int color) {
  unsigned char *pen_8 = dpbuff.buff + y * line_width + x * pixel_width;
  unsigned short *pen_16;
  unsigned int *pen_32;

  unsigned int red, green, blue;

  pen_16 = (unsigned short *)pen_8;
  pen_32 = (unsigned int *)pen_8;

  switch (dpbuff.bpp) {
  case 8: {
    *pen_8 = color;
    break;
  }
  case 16: {
    /* 565 */
    red = (color >> 16) & 0xff;
    green = (color >> 8) & 0xff;
    blue = (color >> 0) & 0xff;
    color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
    *pen_16 = color;
    break;
  }
  case 32: {
    *pen_32 = color;
    break;
  }
  default: {
    printf("can't surport %dbpp\n", dpbuff.bpp);
    break;
  }
  }

  return True;
}

/*
 * 初始化默认的显示器
 */
int init_default_display(void) {
  int ret;

  ret = disp_dft->device_init();
  if (!ret) {
    printf("device init err\n");
    return False;
  }

  ret = disp_dft->get_buffer(&dpbuff);
  if (!ret) {
    printf("device get buffer err\n");
    return False;
  }

  pixel_width = dpbuff.bpp / 8;
  line_width = pixel_width * dpbuff.xres;

  return True;
}

/*
 * 屏幕初始化
 */
void display_init(void) { frame_buffer_init(); }

/*
 * 获取显示buff数据
 */
disp_buff get_display_buffer(void) { return dpbuff; }

/*
 * 注册屏幕
 */
void register_display(disp_ops *dops) {
  dops->next = r_dops;
  r_dops = dops;
}

/*
 * 选择屏幕
 */
int select_default_display(char *name) {
  disp_ops *ptmp = r_dops;

  while (ptmp) {
    if (strcmp(name, ptmp->name) == 0) {
      disp_dft = ptmp;
      return True;
    }
    ptmp = ptmp->next;
  }

  return False;
}

/*
 * 刷新区域
 */
int flush_display_region(region rgn, disp_buff *buffer) {
  return disp_dft->flush_region(rgn, buffer);
}
