#include <input_manger.h>
#include <tslib.h>

static struct tsdev *ts;

// TODO
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

    return 1;
}

// TODO
static int touchscreen_device_init(void) {

  ts = ts_setup(NULL, 0);
  if (!ts) {
    printf("te_setup err\n");
    return -1;
  }

  return 0;
}

// TODO
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