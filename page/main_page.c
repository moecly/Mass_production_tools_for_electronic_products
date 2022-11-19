#include "font_manger.h"
#include "input_manger.h"
#include "sys/time.h"
#include <config.h>
#include <disp_manger.h>
#include <math.h>
#include <page_manger.h>
#include <stdio.h>
#include <string.h>
#include <ui.h>

#define X_GAP 5
#define Y_GAP 5

static button btn[ITEM_CFG_NUM];
static int btn_cnt;

/*
 * main page pressed event.
 */
static void main_page_on_pressed(struct button *btn, disp_buff *buff,
                                 input_event *ievt) {
  unsigned int color = BUTTON_DEFAULT_COLOR;
  char name[100];
  char status[100];
  char *btn_name = btn->name;

  /*
   * get color.
   */
  if (ievt->type == INPUT_TYPE_TOUCH) {
    if (!get_item_cfg_by_name(btn->name)->can_be_touch)
      return;
    btn->status = !btn->status;
    if (btn->status)
      color = BUTTON_PRESSED_COLOR;

  } else if (ievt->type == INPUT_TYPE_NET) {
    sscanf(ievt->str, "%s %s", name, status);
    if (strcmp(status, "ok") == 0)
      color = BUTTON_PRESSED_COLOR;

    else if (strcmp(status, "err") == 0)
      color = BUTTON_DEFAULT_COLOR;

    else if (status[0] >= '0' && status[0] <= '9') {
      color = BUTTON_PERCENT_COLOR;
      btn_name = status;
    } else
      return;
  }

  /*
   * draw region.
   */
  draw_region(btn->region, color);

  /*
   * draw center text.
   */
  draw_text_in_region(btn_name, btn->region, BUTTON_TEXT_COLOR);

  /*
   * flush display region.
   */
  flush_display_region(btn->region, buff);
}

/*
 * generate button by config file.
 */
static int generate_button(void) {
  int width, height;
  int per_line;
  int rows;
  int row, col;
  int n;
  int ret;
  disp_buff buff;
  int xres, yres;
  int start_x, start_y;
  int pre_start_x, pre_start_y;
  button *pbtn;
  region rgn;
  int i = 0;

  /*
   * Calculate the width/height of a single button.
   */
  btn_cnt = n = get_item_cfg_count();
  ret = get_display_buffer(&buff);
  if (ret) {
    printf("get display buffer err\n");
    return -1;
  }

  xres = buff.xres;
  yres = buff.yres;
  width = sqrt((float)xres * yres / n / 0.618);
  per_line = xres / width + 1;
  width = xres / per_line;
  height = 0.618 * width;

  /*
   * clean screen.
   */
  rgn.left_up_x = rgn.left_up_y = 0;
  rgn.width = buff.xres;
  rgn.height = buff.yres;
  draw_region(rgn, WHITE);
  flush_display_region(rgn, &buff);

  /*
   * Center Display.
   */
  start_x = (xres - width * per_line) / 2;
  rows = n / per_line;
  if (rows * per_line < n)
    rows++;

  start_y = (yres - height * rows) / 2;
  for (row = 0; (row < rows) && (i < n); row++) {
    pre_start_x = start_x - width; // ?
    pre_start_y = row * height + start_y;
    for (col = 0; (col < per_line) && (i < n); col++) {
      pbtn = &btn[i];
      pbtn->region.left_up_x = pre_start_x + width;
      pbtn->region.left_up_y = pre_start_y;
      pbtn->region.width = width - X_GAP;
      pbtn->region.height = height - Y_GAP;
      pre_start_x = pbtn->region.left_up_x;

      init_button(pbtn, get_item_cfg_by_index(i++)->name, NULL, NULL,
                  main_page_on_pressed);
    }
  }

  for (i = 0; i < n; i++) {
    btn[i].on_draw(&btn[i], &buff, BUTTON_DEFAULT_COLOR);
  }

  return 0;
}

/*
 * get touch point in where region.
 */
static int is_touch_point_in_region(int x, int y, region *rgn) {
  if (x < rgn->left_up_x || x >= rgn->left_up_x + rgn->width)
    return 0;

  if (y < rgn->left_up_y || y >= rgn->left_up_y + rgn->height)
    return 0;

  return 1;
}

/*
 * find button by name.
 */
static button *get_button_by_name(char *name) {
  int i = 0;

  for (; i < btn_cnt; i++) {
    if (strcmp(btn[i].name, name) == 0)
      return &btn[i];
  }

  return NULL;
}

/*
 * get button by input event.
 */
static button *get_button_by_input_event(input_event *ievt) {
  int i = 0;
  char name[100];

  if (ievt->type == INPUT_TYPE_TOUCH) {
    for (; i < btn_cnt; i++) {
      if (is_touch_point_in_region(ievt->x, ievt->y, &btn[i].region))
        return &btn[i];
    }
  } else if (ievt->type == INPUT_TYPE_NET) {
    sscanf(ievt->str, "%s", name);
    return get_button_by_name(name);
  }

  return NULL;
}

/*
 * main page run.
 */
static int main_page_run(void *params) {
  int ret;
  input_event ievt;
  button *pbtn;
  disp_buff buff;

  /*
   * Read configuration file.
   */
  ret = parse_config_file();
  if (ret) {
    printf("parse config file err\n");
    return -1;
  }

  /*
   * Generate interface according to configuration file.
   */
  generate_button();

  /*
   * read input event.
   */
  ret = get_display_buffer(&buff);
  if (ret) {
    return -1;
  }

  while (1) {
    ret = get_input_event(&ievt);
    if (ret)
      continue;

    pbtn = get_button_by_input_event(&ievt);
    if (!pbtn)
      continue;

    btn->on_pressed(pbtn, &buff, &ievt);
  }
}

static page_action page_atn = {
    .name = "main",
    .run = main_page_run,
};

/*
 * register main page.
 */
void main_page_register(void) { register_page(&page_atn); }
