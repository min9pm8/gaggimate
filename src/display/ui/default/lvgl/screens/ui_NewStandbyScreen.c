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
    lv_obj_set_style_bg_color(ui_NewStandbyScreen, UI_COLOR_BG, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewStandbyScreen, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_NewStandbyScreen, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(ui_NewStandbyScreen, 0, LV_PART_MAIN);

    // Tap anywhere -> brew
    lv_obj_add_flag(ui_NewStandbyScreen, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui_NewStandbyScreen, ui_event_NewStandbyScreen, LV_EVENT_CLICKED, NULL);

    // --- Centered WiFi icon ---
    ui_NewStandbyScreen_wifiIcon = lv_label_create(ui_NewStandbyScreen);
    lv_label_set_text(ui_NewStandbyScreen_wifiIcon, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_font(ui_NewStandbyScreen_wifiIcon, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewStandbyScreen_wifiIcon, UI_COLOR_STANDBY_ICON_PRI, LV_PART_MAIN);
    lv_obj_center(ui_NewStandbyScreen_wifiIcon);
}
