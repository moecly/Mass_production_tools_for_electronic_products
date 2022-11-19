#ifndef _FONT_MANGER_H
#define _FONT_MANGER_H

#ifndef NULL
#define NULL (void *)0
#endif

typedef struct region {
  int left_up_x;
  int left_up_y;
  int width;
  int height;
} region;

typedef struct font_bit_map {
  region region;
  int cur_origin_x;
  int cur_origin_y;
  int next_origin_x;
  int next_origin_y;
  unsigned char *buffer;
} font_bit_map;

typedef struct font_ops {
  char *name;
  int (*font_init)(char *);
  int (*set_font_size)(unsigned int);
  int (*get_font_bit_map)(unsigned int, font_bit_map *);
  struct font_ops *next;
} font_ops;

void register_font(font_ops *fops);
void fonts_register(void);
int get_font_bit_map(unsigned int code, font_bit_map *fb_map);
int fonts_init(char *font_name);
int sel_def_font(char *name);
int set_font_size(unsigned int size);

#endif