#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <disp_manger.h>

static int fd_fb;
static struct fb_var_screeninfo var;
static int screen_size;
static unsigned char *fb_base;
static unsigned int line_width;
static unsigned int pixel_width;

/*
 * 初始化lcd屏幕
 */
static int lcd_device_init(void) {
  int ret;

  fd_fb = open("/dev/fb0", O_RDWR);
  if (fd_fb < 0) {
    printf("can't open /dev/fb0\n");
    return False;
  }

  ret = ioctl(fd_fb, FBIOGET_VSCREENINFO, &var);
  if (ret < 0) {
    printf("ioctl fd_fb err\n");
    close(fd_fb);
    return False;
  }

  /*
   * 获取屏幕信息
   */
  pixel_width = var.bits_per_pixel / 8;
  line_width = pixel_width * var.xres;
  screen_size = var.xres * var.yres * pixel_width;

  /*
   * 映射地址
   */
  fb_base =
      mmap(NULL, screen_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd_fb, 0);
  if (fb_base == MAP_FAILED) {
    printf("mmap fd_fb err\n");
    close(fd_fb);
    return False;
  }

  return True;
}

/*
 * 取消映射，关闭文件，退出lcd
 */
static void lcd_device_exit(void) {
  munmap(fb_base, screen_size);
  close(fd_fb);
}

/*
 * 获取屏幕framebuffer地址
 */
static int lcd_get_buffer(disp_buff *dpbuff) {
  dpbuff->xres = var.xres;
  dpbuff->yres = var.yres;
  dpbuff->bpp = var.bits_per_pixel;
  dpbuff->buff = fb_base;
  return True;
}

// TODO
static int lcd_flush_region(region rgn, disp_buff *buffer) { return 0; }

disp_ops lcd_disp_ops = {
    .name = "lcd",
    .device_init = lcd_device_init,
    .flush_region = lcd_flush_region,
    .get_buffer = lcd_get_buffer,
    .device_exit = lcd_device_exit,
};

/*
 * 注册屏幕
 */
void frame_buffer_init(void) { register_display(&lcd_disp_ops); }