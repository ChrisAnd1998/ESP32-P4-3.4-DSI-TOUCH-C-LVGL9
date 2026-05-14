#include "displays_config.h"
#include "gt911.h"
#include <lvgl.h>
#include "lv_conf.h"

Arduino_ESP32DSIPanel *dsipanel = new Arduino_ESP32DSIPanel(
  display_cfg.hsync_pulse_width,
  display_cfg.hsync_back_porch,
  display_cfg.hsync_front_porch,
  display_cfg.vsync_pulse_width,
  display_cfg.vsync_back_porch,
  display_cfg.vsync_front_porch,
  display_cfg.prefer_speed,
  display_cfg.lane_bit_rate);

Arduino_DSI_Display *gfx = new Arduino_DSI_Display(
  display_cfg.width,
  display_cfg.height,
  dsipanel,
  0,
  true,
  -1,
  display_cfg.init_cmds,
  display_cfg.init_cmds_size);

// Touch
static esp_lcd_touch_handle_t tp_handle = NULL;
#define MAX_TOUCH_POINTS 5
static uint16_t touch_x[MAX_TOUCH_POINTS];
static uint16_t touch_y[MAX_TOUCH_POINTS];
static uint16_t touch_strength[MAX_TOUCH_POINTS];
static uint8_t touch_cnt = 0;
static bool touch_pressed = false;

// LVGL buffers
#define LVGL_TICK_PERIOD 5
#define DRAW_BUF_HEIGHT 80
static lv_display_t *lv_display;
static lv_indev_t *indev_touchpad;
static lv_color_t *lv_draw_buf1;
static lv_color_t *lv_draw_buf2;

// ---------------------
// LVGL callbacks
// ---------------------
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t*)px_map, w, h);
  lv_display_flush_ready(disp);
}

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  esp_lcd_touch_read_data(tp_handle);
  touch_pressed = esp_lcd_touch_get_coordinates(tp_handle, touch_x, touch_y, touch_strength, &touch_cnt, MAX_TOUCH_POINTS);

  if (touch_pressed && touch_cnt > 0) {
    data->point.x = touch_x[0];
    data->point.y = touch_y[0];
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void lvglTick(void *param) {
  lv_tick_inc(LVGL_TICK_PERIOD);
}

void displayTask(void *parameter)
{
  while (true)
  {
    lv_timer_handler();
    vTaskDelay(pdMS_TO_TICKS(1)); // yield but don’t block long
  }
}

void displayInit() {
  DEV_I2C_Port port = DEV_I2C_Init();
  display_init(port);
  tp_handle = touch_gt911_init(port);

  gfx->begin();
  set_display_backlight(port, 255);

  gfx->fillScreen(RGB565_BLACK);

  lv_init();
  size_t draw_buf_size = display_cfg.width * DRAW_BUF_HEIGHT;
  lv_draw_buf1 = (lv_color_t*)heap_caps_malloc(draw_buf_size * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
  lv_draw_buf2 = (lv_color_t*)heap_caps_malloc(draw_buf_size * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);

  lv_display = lv_display_create(display_cfg.width, display_cfg.height);
  lv_display_set_flush_cb(lv_display, my_disp_flush);
  lv_display_set_buffers(lv_display, lv_draw_buf1, lv_draw_buf2, draw_buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

  indev_touchpad = lv_indev_create();
  lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev_touchpad, my_touchpad_read);

  const esp_timer_create_args_t lvgl_timer_args = {.callback = &lvglTick, .name = "lvgl_timer"};
  esp_timer_handle_t lvgl_timer;
  esp_timer_create(&lvgl_timer_args, &lvgl_timer);
  esp_timer_start_periodic(lvgl_timer, LVGL_TICK_PERIOD * 1000);


  xTaskCreatePinnedToCore(
    displayTask,
    "Display Task",
    8192,      // ✅ minimum
    NULL,
    2,
    NULL,
    1          // also move to core 1 (explained below)
  );
}
