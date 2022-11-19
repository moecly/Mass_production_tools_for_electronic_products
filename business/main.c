#include <disp_manger.h>
#include <font_manger.h>
#include <input_manger.h>
#include <page_manger.h>
#include <stdio.h>
#include <ui.h>

int main(int argc, char **argv) {
  int ret;
  disp_buff buff;

  /*
   * init lcd display.
   */
  display_init();
  ret = select_default_display("lcd");
  if (ret) {
    printf("select default display err\n");
    return -1;
  }
  ret = init_default_display();
  if (ret) {
    printf("init default display err\n");
  }

  ret = get_display_buffer(&buff);
  if (ret) {
    printf("get display buffer err\n");
    return -1;
  }

  /*
   * init network input and lcd input.
   */
  input_init();
  input_device_init();

  /*
   * init font.
   */
  fonts_register();
  fonts_init("simsun.ttc");
  sel_def_font("freetype");

  /*
   * init page.
   */
  pages_register();
  page("main")->run(NULL);

  return 0;
}