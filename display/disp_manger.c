#include "font_manger.h"
#include <disp_manger.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static disp_ops *r_dops;
static disp_ops *disp_dft;
static disp_buff dp_buff;
static int line_width;
static int pixel_width;

extern void frame_buffer_init(void);

/*
 * draw pixel.
 */
int put_pixel(int x, int y, unsigned int color) {
  unsigned char *pen_8 = dp_buff.buff + y * line_width + x * pixel_width;
  unsigned short *pen_16;
  unsigned int *pen_32;

  unsigned int red, green, blue;

  pen_16 = (unsigned short *)pen_8;
  pen_32 = (unsigned int *)pen_8;

  switch (dp_buff.bpp) {
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
    printf("can't surport %dbpp\n", dp_buff.bpp);
    break;
  }
  }

  return True;
}

/*
 * draw font bit map.
 */
void draw_from_bit_map(font_bit_map fb_map, unsigned int color) {
  int p, q;
  int i, j;
  int x = fb_map.region.left_up_x;
  int y = fb_map.region.left_up_y;
  int x_max = fb_map.region.width;
  int y_max = fb_map.region.height;

  for (j = y, q = 0; q < y_max; j++, q++) {
    for (i = x, p = 0; p < x_max; i++, p++) {
      if (i < 0 || j < 0 || i >= dp_buff.xres || j >= dp_buff.yres) {
        continue;
      }
      if (fb_map.buffer[q * fb_map.region.width + p])
        put_pixel(i, j, color);
    }
  }
}

/*
 * draw text in region center.
 */
int draw_text_in_region(char *name, region rgn, unsigned int color) {
  int ret;
  int n = strlen(name);
  int font_size = rgn.width / (n * 2);
  font_bit_map fb_map;

  if (font_size > rgn.height)
    font_size = rgn.height;
  fb_map.cur_origin_x = (rgn.width - font_size * n) / 2 + rgn.left_up_x;
  fb_map.cur_origin_y =
      (rgn.height - font_size) / 2 + rgn.left_up_y + font_size;

  set_font_size(font_size);
  while (*name) {
    ret = get_font_bit_map((unsigned int)*name, &fb_map);
    name++;
    if (ret) {
      return -1;
    }

    draw_from_bit_map(fb_map, color);
    fb_map.cur_origin_x = fb_map.next_origin_x;
    fb_map.cur_origin_y = fb_map.next_origin_y;
  }

  return 0;
}

/*
 * draw region.
 */
void draw_region(region reg, unsigned int color) {
  int x = reg.left_up_x;
  int y = reg.left_up_y;
  int width = reg.width;
  int height = reg.height;
  int i, j;

  for (j = y; j < y + height; j++) {
    for (i = x; i < x + width; i++) {
      if (i < 0 || j < 0 || i >= dp_buff.xres || j >= dp_buff.yres)
        continue;
      put_pixel(i, j, color);
    }
  }
}

/*
 * init default display device.
 * need to use int "select_default_display(char *name)" to select default
 * device.
 */
int init_default_display(void) {
  int ret;

  ret = disp_dft->device_init();
  if (ret) {
    printf("device init err\n");
    return False;
  }

  ret = disp_dft->get_buffer(&dp_buff);
  if (ret) {
    printf("device get buffer err\n");
    return False;
  }

  pixel_width = dp_buff.bpp / 8;
  line_width = pixel_width * dp_buff.xres;

  return True;
}

/*
 * display device init.
 */
void display_init(void) { frame_buffer_init(); }

/*
 * get display buffer.
 */
int get_display_buffer(disp_buff *buff) {
  *buff = dp_buff;
  return 0;
}

/*
 * register device linked list.
 */
void register_display(disp_ops *dops) {
  dops->next = r_dops;
  r_dops = dops;
}

/*
 * select default device.
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
 * flush display region.
 */
int flush_display_region(region rgn, disp_buff *buffer) {
  return disp_dft->flush_region(rgn, buffer);
}
