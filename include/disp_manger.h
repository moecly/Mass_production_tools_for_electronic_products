#ifndef _DISP_MANGER_H
#define _DISP_MANGER_H

#define True 0
#define False -1

#include <font_manger.h>

typedef struct disp_buff {
  int xres;
  int yres;
  int bpp;
  unsigned char *buff;
} disp_buff;

typedef struct display_operations {
  char *name;
  int (*device_init)(void);
  void (*device_exit)(void);
  int (*get_buffer)(disp_buff *);
  int (*flush_region)(region, disp_buff *);
  struct display_operations *next;
} disp_ops;

void register_display(disp_ops *dops);
void display_init(void);
int draw_text_in_region(char *name, region rgn, unsigned int color);
int init_default_display(void);
int select_default_display(char *name);
int flush_display_region(region rgn, disp_buff *buffer);
int put_pixel(int x, int y, unsigned int color);
int get_display_buffer(disp_buff *buff);
void draw_region(region reg, unsigned int color);
void draw_from_bit_map(font_bit_map fb_map, unsigned int color);

#endif