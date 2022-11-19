#ifndef _UI_H
#define _UI_H

#include "disp_manger.h"
#include "input_manger.h"

#define WHITE 0xFFFFFF
#define BUTTON_DEFAULT_COLOR 0xFF0000
#define BUTTON_PRESSED_COLOR 0x00FF00
#define BUTTON_PERCENT_COLOR 0x0000FF
#define BUTTON_TEXT_COLOR 0x000000

typedef struct button {
  char *name;
  char status;
  region region;
  void (*on_draw)(struct button *, disp_buff *, unsigned int);
  void (*on_pressed)(struct button *, disp_buff *, input_event *);
} button;

typedef void (*on_draw)(button *, disp_buff *, unsigned int);
typedef void (*on_pressed)(button *, disp_buff *, input_event *);

void default_on_draw(button *btn, disp_buff *buff, unsigned int color);
void default_on_pressed(button *btn, disp_buff *buff, input_event *ievt);
int init_button(button *btn, char *name, region *rgn, on_draw draw,
                on_pressed pred);

#endif