#include "ui_NewStandbyScreen.h"
#include "../ui_new_colors.h"
#include "../ui_new_events.h"
#include "../ui.h"

lv_obj_t *ui_NewStandbyScreen = NULL;
lv_obj_t *ui_NewStandbyScreen_wifiIcon = NULL;
lv_obj_t *ui_NewStandbyScreen_btIcon = NULL;

void ui_NewStandbyScreen_screen_init(void) {
    ui_NewStandbyScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_NewStandbyScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_NewStandbyScreen, UI_COLOR_STANDBY_BG, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewStandbyScreen, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(ui_NewStandbyScreen, LV_RADIUS_CIRCLE, LV_PART_MAIN);

    // Container for centering icons
    lv_obj_t *icon_container = lv_obj_create(ui_NewStandbyScreen);
    lv_obj_remove_style_all(icon_container);
    lv_obj_set_size(icon_container, UI_STANDBY_ICON_SIZE * 2 + UI_STANDBY_ICON_GAP, UI_STANDBY_ICON_SIZE);
    lv_obj_center(icon_container);
    lv_obj_set_flex_flow(icon_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(icon_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(icon_container, UI_STANDBY_ICON_GAP, LV_PART_MAIN);

    // WiFi icon — use existing asset scaled up
    ui_NewStandbyScreen_wifiIcon = lv_img_create(icon_container);
    lv_img_set_src(ui_NewStandbyScreen_wifiIcon, &ui_img_364513079); // wifi-20x20.png
    lv_img_set_zoom(ui_NewStandbyScreen_wifiIcon, 256 * 64 / 20);   // Scale 20px -> 64px
    lv_obj_set_style_img_recolor(ui_NewStandbyScreen_wifiIcon, UI_COLOR_STANDBY_ICON_PRI, LV_PART_MAIN);
    lv_obj_set_style_img_recolor_opa(ui_NewStandbyScreen_wifiIcon, LV_OPA_COVER, LV_PART_MAIN);

    // Bluetooth icon
    ui_NewStandbyScreen_btIcon = lv_img_create(icon_container);
    lv_img_set_src(ui_NewStandbyScreen_btIcon, &ui_img_1091371356); // bluetooth-alt-20x20.png
    lv_img_set_zoom(ui_NewStandbyScreen_btIcon, 256 * 64 / 20);
    lv_obj_set_style_img_recolor(ui_NewStandbyScreen_btIcon, UI_COLOR_STANDBY_ICON_SEC, LV_PART_MAIN);
    lv_obj_set_style_img_recolor_opa(ui_NewStandbyScreen_btIcon, UI_STANDBY_ICON_SEC_OPA, LV_PART_MAIN);

    // Tap anywhere → Brew screen
    lv_obj_add_event_cb(ui_NewStandbyScreen, ui_event_NewStandbyScreen, LV_EVENT_CLICKED, NULL);
}
