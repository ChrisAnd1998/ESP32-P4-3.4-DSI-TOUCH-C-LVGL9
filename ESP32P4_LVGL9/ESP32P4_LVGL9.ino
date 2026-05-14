// Tools -> PSRAM -> Enabled
// Tools -> Partition Scheme -> Huge App

#include "lvgl_display.h"

lv_obj_t *scr_main;
lv_obj_t *scr_menu;
lv_obj_t *scr_stations;
lv_obj_t *scr_local;
lv_obj_t *label_title;
lv_obj_t *label_artist;
lv_obj_t *arc_progress;
lv_obj_t *label_time;
lv_obj_t *slider_volume;
static lv_image_dsc_t album_img_dsc;
lv_obj_t *album_img;

bool skipping = false;
bool paused = false;

#include "sd.h"

#include "lvgl_screen_main.h"
#include "lvgl_screen_menu.h"

void setup() {
  Serial.begin(115200);

  displayInit();
  sdInit();

  create_main_screen();
  create_menu_screen();

  delay(1000);

  lv_scr_load(scr_main);
}

void loop(){
  delay(100);
}