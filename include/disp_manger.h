#ifndef _DISP_MANGER_H
#define _DISP_MANGER_H

#define True 1
#define False 0

typedef struct disp_buff {
  int xres;
  int yres;
  int bpp;
  unsigned char *buff;
} disp_buff;

typedef struct region {
  int left_up_x;
  int left_up_y;
  int width;
  int height;
} region;

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
int init_default_display(void);
int select_default_display(char *name);
int flush_display_region(region rgn, disp_buff *buffer);
int put_pixel(int x, int y, unsigned int color);
disp_buff get_display_buffer(void);

#endif