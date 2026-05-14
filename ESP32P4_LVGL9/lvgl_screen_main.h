static void onMenu(lv_event_t *e) {
   lv_scr_load(scr_menu);
}

void create_main_screen()
{
    scr_main = lv_obj_create(NULL);
    lv_obj_set_size(scr_main, 800, 800);
    lv_obj_set_style_bg_color(scr_main, lv_color_white(), 0);
    lv_obj_set_style_border_width(scr_main, 0, 0);

    lv_obj_t *label = lv_label_create(scr_main);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
    lv_label_set_text(label, "Hello Arduino, I'm LVGL!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    /* =============================
       Menu Button
    ============================== */

    // Menu
    lv_obj_t *btn_menu = lv_btn_create(scr_main);
    lv_obj_set_size(btn_menu, 100, 100);
    lv_obj_align(btn_menu, LV_ALIGN_CENTER, -250, 150);
    lv_obj_set_style_radius(btn_menu, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(btn_menu, lv_color_hex(0x555555), 0);
    lv_obj_add_event_cb(btn_menu, onMenu, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label_menu = lv_label_create(btn_menu);
    lv_obj_set_style_text_font(label_menu, &lv_font_montserrat_28, 0);
    lv_label_set_text(label_menu, "Menu");
    lv_obj_center(label_menu);

}