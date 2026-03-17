#include "ui_NewStandbyScreen.h"
#include "../ui_new_colors.h"
#include "../ui_new_events.h"
#include "../ui.h"

lv_obj_t *ui_NewStandbyScreen = NULL;
lv_obj_t *ui_NewStandbyScreen_wifiIcon = NULL;
lv_obj_t *ui_NewStandbyScreen_btIcon = NULL;
lv_obj_t *ui_NewStandbyScreen_waterZone = NULL;
lv_obj_t *ui_NewStandbyScreen_brewZone = NULL;
lv_obj_t *ui_NewStandbyScreen_steamZone = NULL;

void ui_NewStandbyScreen_screen_init(void) {
    ui_NewStandbyScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_NewStandbyScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_NewStandbyScreen, UI_COLOR_STANDBY_BG, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewStandbyScreen, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_NewStandbyScreen, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(ui_NewStandbyScreen, 0, LV_PART_MAIN);

    // --- Top center: WiFi + BT status icons (small) ---
    lv_obj_t *status_bar = lv_obj_create(ui_NewStandbyScreen);
    lv_obj_remove_style_all(status_bar);
    lv_obj_set_size(status_bar, 120, 30);
    lv_obj_align(status_bar, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_set_flex_flow(status_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(status_bar, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(status_bar, 12, LV_PART_MAIN);

    ui_NewStandbyScreen_wifiIcon = lv_label_create(status_bar);
    lv_label_set_text(ui_NewStandbyScreen_wifiIcon, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_font(ui_NewStandbyScreen_wifiIcon, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewStandbyScreen_wifiIcon, UI_COLOR_STANDBY_ICON_PRI, LV_PART_MAIN);

    ui_NewStandbyScreen_btIcon = lv_label_create(status_bar);
    lv_label_set_text(ui_NewStandbyScreen_btIcon, LV_SYMBOL_BLUETOOTH);
    lv_obj_set_style_text_font(ui_NewStandbyScreen_btIcon, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewStandbyScreen_btIcon, UI_COLOR_STANDBY_ICON_SEC, LV_PART_MAIN);
    lv_obj_set_style_text_opa(ui_NewStandbyScreen_btIcon, UI_STANDBY_ICON_SEC_OPA, LV_PART_MAIN);

    // --- Three-zone row container ---
    lv_obj_t *zone_row = lv_obj_create(ui_NewStandbyScreen);
    lv_obj_remove_style_all(zone_row);
    lv_obj_set_size(zone_row, 380, 260);
    lv_obj_align(zone_row, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_flex_flow(zone_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(zone_row, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // --- Helper: create a zone (column with icon + label) ---
    // Water zone (left)
    ui_NewStandbyScreen_waterZone = lv_obj_create(zone_row);
    lv_obj_remove_style_all(ui_NewStandbyScreen_waterZone);
    lv_obj_set_size(ui_NewStandbyScreen_waterZone, 120, 200);
    lv_obj_set_flex_flow(ui_NewStandbyScreen_waterZone, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_NewStandbyScreen_waterZone, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(ui_NewStandbyScreen_waterZone, 8, LV_PART_MAIN);
    lv_obj_add_flag(ui_NewStandbyScreen_waterZone, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui_NewStandbyScreen_waterZone, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *waterCircle = lv_obj_create(ui_NewStandbyScreen_waterZone);
    lv_obj_remove_style_all(waterCircle);
    lv_obj_set_size(waterCircle, 100, 100);
    lv_obj_set_style_radius(waterCircle, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(waterCircle, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(waterCircle, UI_OPA(10), LV_PART_MAIN);
    lv_obj_set_style_border_color(waterCircle, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_border_width(waterCircle, 2, LV_PART_MAIN);
    lv_obj_clear_flag(waterCircle, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *waterIcon = lv_label_create(waterCircle);
    lv_label_set_text(waterIcon, LV_SYMBOL_TINT);
    lv_obj_set_style_text_font(waterIcon, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(waterIcon, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_center(waterIcon);

    lv_obj_t *waterLabel = lv_label_create(ui_NewStandbyScreen_waterZone);
    lv_label_set_text(waterLabel, "WATER");
    lv_obj_set_style_text_font(waterLabel, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(waterLabel, UI_COLOR_TEXT_SEC, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(waterLabel, 2, LV_PART_MAIN);

    lv_obj_add_event_cb(ui_NewStandbyScreen_waterZone, ui_event_StandbyZone_water, LV_EVENT_CLICKED, NULL);

    // Brew zone (center)
    ui_NewStandbyScreen_brewZone = lv_obj_create(zone_row);
    lv_obj_remove_style_all(ui_NewStandbyScreen_brewZone);
    lv_obj_set_size(ui_NewStandbyScreen_brewZone, 120, 200);
    lv_obj_set_flex_flow(ui_NewStandbyScreen_brewZone, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_NewStandbyScreen_brewZone, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(ui_NewStandbyScreen_brewZone, 8, LV_PART_MAIN);
    lv_obj_add_flag(ui_NewStandbyScreen_brewZone, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui_NewStandbyScreen_brewZone, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *brewCircle = lv_obj_create(ui_NewStandbyScreen_brewZone);
    lv_obj_remove_style_all(brewCircle);
    lv_obj_set_size(brewCircle, 100, 100);
    lv_obj_set_style_radius(brewCircle, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(brewCircle, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(brewCircle, UI_OPA(10), LV_PART_MAIN);
    lv_obj_set_style_border_color(brewCircle, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_border_width(brewCircle, 2, LV_PART_MAIN);
    lv_obj_clear_flag(brewCircle, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *brewIcon = lv_label_create(brewCircle);
    lv_label_set_text(brewIcon, LV_SYMBOL_PLAY);
    lv_obj_set_style_text_font(brewIcon, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(brewIcon, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_center(brewIcon);

    lv_obj_t *brewLabel = lv_label_create(ui_NewStandbyScreen_brewZone);
    lv_label_set_text(brewLabel, "BREW");
    lv_obj_set_style_text_font(brewLabel, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(brewLabel, UI_COLOR_TEXT_SEC, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(brewLabel, 2, LV_PART_MAIN);

    lv_obj_add_event_cb(ui_NewStandbyScreen_brewZone, ui_event_StandbyZone_brew, LV_EVENT_CLICKED, NULL);

    // Steam zone (right)
    ui_NewStandbyScreen_steamZone = lv_obj_create(zone_row);
    lv_obj_remove_style_all(ui_NewStandbyScreen_steamZone);
    lv_obj_set_size(ui_NewStandbyScreen_steamZone, 120, 200);
    lv_obj_set_flex_flow(ui_NewStandbyScreen_steamZone, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_NewStandbyScreen_steamZone, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(ui_NewStandbyScreen_steamZone, 8, LV_PART_MAIN);
    lv_obj_add_flag(ui_NewStandbyScreen_steamZone, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui_NewStandbyScreen_steamZone, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *steamCircle = lv_obj_create(ui_NewStandbyScreen_steamZone);
    lv_obj_remove_style_all(steamCircle);
    lv_obj_set_size(steamCircle, 100, 100);
    lv_obj_set_style_radius(steamCircle, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(steamCircle, UI_COLOR_AMBER, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(steamCircle, UI_OPA(10), LV_PART_MAIN);
    lv_obj_set_style_border_color(steamCircle, UI_COLOR_AMBER, LV_PART_MAIN);
    lv_obj_set_style_border_width(steamCircle, 2, LV_PART_MAIN);
    lv_obj_clear_flag(steamCircle, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *steamIcon = lv_label_create(steamCircle);
    lv_label_set_text(steamIcon, LV_SYMBOL_UP);
    lv_obj_set_style_text_font(steamIcon, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(steamIcon, UI_COLOR_AMBER, LV_PART_MAIN);
    lv_obj_center(steamIcon);

    lv_obj_t *steamLabel = lv_label_create(ui_NewStandbyScreen_steamZone);
    lv_label_set_text(steamLabel, "STEAM");
    lv_obj_set_style_text_font(steamLabel, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(steamLabel, UI_COLOR_TEXT_SEC, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(steamLabel, 2, LV_PART_MAIN);

    lv_obj_add_event_cb(ui_NewStandbyScreen_steamZone, ui_event_StandbyZone_steam, LV_EVENT_CLICKED, NULL);
}
