static void onBack(lv_event_t *e) {
   lv_obj_t * current = lv_scr_act();

    if (current == scr_menu) {
        lv_scr_load(scr_main);
    } else {
        lv_scr_load(scr_menu);
    }
}

void create_menu_screen() {
    scr_menu = lv_obj_create(NULL);
    lv_obj_set_size(scr_menu, 800, 800);
    lv_obj_set_style_bg_color(scr_menu, lv_color_black(), 0);
    lv_obj_set_style_border_width(scr_menu, 0, 0);

    // Back
    lv_obj_t *btn_back = lv_btn_create(scr_menu);
    lv_obj_set_size(btn_back, 100, 100);
    lv_obj_align(btn_back, LV_ALIGN_CENTER, 0, 310);
    lv_obj_set_style_radius(btn_back, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(btn_back, lv_color_hex(0x555555), 0);
    lv_obj_add_event_cb(btn_back, onBack, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label_back = lv_label_create(btn_back);
    lv_obj_set_style_text_font(label_back, &lv_font_montserrat_28, 0);
    lv_label_set_text(label_back, "Back");
    lv_obj_set_style_text_color(label_back, lv_color_white(), 0);
    lv_obj_center(label_back);


}