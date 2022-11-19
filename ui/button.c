#include "font_manger.h"
#include "input_manger.h"
#include <disp_manger.h>
#include <ui.h>
#include <stdio.h>

/*
 * button default draw.
 */
void default_on_draw(button *btn, disp_buff *buff, unsigned int color) {
  /*
   * draw color.
   */
  draw_region(btn->region, color);

  /*
   * draw text.
   */
  draw_text_in_region(btn->name, btn->region, BUTTON_TEXT_COLOR);

  /*
   * flush to lcd/web.
   */
  flush_display_region(btn->region, buff);
}

/*
 * default pressed function.
 */
void default_on_pressed(button *btn, disp_buff *buff, input_event *ievt) {
  unsigned int color;

  btn->status = !btn->status;
  if (btn->status)
    color = BUTTON_PRESSED_COLOR;
  else
    color = BUTTON_DEFAULT_COLOR;

  default_on_draw(btn, buff, color);
}

/*
 * init a button.
 */
int init_button(button *btn, char *name, region *rgn, on_draw draw,
                on_pressed pred) {
  btn->name = name;
  btn->on_draw = draw ? draw : default_on_draw;
  btn->on_pressed = pred ? pred : default_on_pressed;
  if (rgn)
    btn->region = *rgn;
  btn->status = 0;
  return 0;
}
