#include <input_manger.h>
#include <tslib.h>
#include <stdio.h>

static struct tsdev *ts;

/* 
 * 获取触摸事件
 */
static int touchscreen_get_input_event(input_event *ievent) {
    struct ts_sample samp;
    int ret;

    ret = ts_read(ts, &samp, 1);
    if (ret != 1) {
        printf("ts_read err\n");
        return -1;
    }

    ievent->x = samp.x;
    ievent->y = samp.y;
    ievent->pressure = samp.pressure;
    ievent->time = samp.tv;
    ievent->type = INPUT_TYPE_TOUCH;

    return 0;
}

/* 
 * 初始化触摸屏设备
 */
static int touchscreen_device_init(void) {

  ts = ts_setup(NULL, 0);
  if (!ts) {
    printf("te_setup err\n");
    return -1;
  }

  return 0;
}

/* 
 * 退出触摸屏设备
 */
static int touchscreen_device_exit(void) {
  ts_close(ts);
  return 0;
}

input_device idev = {
    .name = "touchscreen",
    .device_init = touchscreen_device_init,
    .device_exit = touchscreen_device_exit,
    .get_input_event = touchscreen_get_input_event,
};

#if 1

/* 
 * 测试
 */
int main(int argc, char **argv) {
  input_event ievt;
  int ret;

  idev.device_init();
  while (1) {
    ret = idev.get_input_event(&ievt);
    if (ret) {
      printf("get input event err\n");
      return -1;
    }

    printf("type: %d", ievt.type);
    printf("x: %d", ievt.x);
    printf("y: %d", ievt.y);
    printf("pressure: %d", ievt.pressure);
  }

  return 0;
}

#endif 