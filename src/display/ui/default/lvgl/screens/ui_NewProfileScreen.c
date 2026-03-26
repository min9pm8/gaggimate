#include "ui_NewProfileScreen.h"
#include "../ui_new_colors.h"
#include "../ui_new_events.h"
#include "../ui.h"

lv_obj_t *ui_NewProfileScreen = NULL;
lv_obj_t *ui_NewProfileScreen_card1 = NULL;
lv_obj_t *ui_NewProfileScreen_card2 = NULL;
lv_obj_t *ui_NewProfileScreen_name1 = NULL;
lv_obj_t *ui_NewProfileScreen_name2 = NULL;
lv_obj_t *ui_NewProfileScreen_detail1 = NULL;
lv_obj_t *ui_NewProfileScreen_detail2 = NULL;

void ui_NewProfileScreen_screen_init(void) {
    ui_NewProfileScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_NewProfileScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_NewProfileScreen, UI_COLOR_BG, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewProfileScreen, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_NewProfileScreen, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(ui_NewProfileScreen, 0, LV_PART_MAIN);

    // Swipe down → go back to standby
    lv_obj_add_flag(ui_NewProfileScreen, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(ui_NewProfileScreen, ui_event_NewProfileScreen_gesture, LV_EVENT_GESTURE, NULL);

    // --- Centered column (matches brew screen center_stack pattern) ---
    lv_obj_t *col = lv_obj_create(ui_NewProfileScreen);
    lv_obj_remove_style_all(col);
    lv_obj_set_size(col, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(col);
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(col, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(col, 24, LV_PART_MAIN);

    // --- Profile 1: UP arrow circle (green/primary) ---
    ui_NewProfileScreen_card1 = lv_btn_create(col);
    lv_obj_set_size(ui_NewProfileScreen_card1, UI_BREW_BTN_SIZE, UI_BREW_BTN_SIZE);
    lv_obj_set_style_radius(ui_NewProfileScreen_card1, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewProfileScreen_card1, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewProfileScreen_card1, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(ui_NewProfileScreen_card1, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(ui_NewProfileScreen_card1, LV_OPA_30, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_NewProfileScreen_card1, 20, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_NewProfileScreen_card1, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(ui_NewProfileScreen_card1, ui_event_NewProfileScreen_card1, LV_EVENT_CLICKED, NULL);

    ui_NewProfileScreen_name1 = lv_label_create(ui_NewProfileScreen_card1);
    lv_label_set_text(ui_NewProfileScreen_name1, LV_SYMBOL_UP);
    lv_obj_set_style_text_font(ui_NewProfileScreen_name1, &lv_font_montserrat_34, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewProfileScreen_name1, UI_COLOR_BG, LV_PART_MAIN);
    lv_obj_center(ui_NewProfileScreen_name1);

    // --- Profile 2: DOWN arrow circle (blue/secondary) ---
    ui_NewProfileScreen_card2 = lv_btn_create(col);
    lv_obj_set_size(ui_NewProfileScreen_card2, UI_BREW_BTN_SIZE, UI_BREW_BTN_SIZE);
    lv_obj_set_style_radius(ui_NewProfileScreen_card2, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewProfileScreen_card2, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewProfileScreen_card2, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(ui_NewProfileScreen_card2, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(ui_NewProfileScreen_card2, LV_OPA_30, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_NewProfileScreen_card2, 20, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_NewProfileScreen_card2, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(ui_NewProfileScreen_card2, ui_event_NewProfileScreen_card2, LV_EVENT_CLICKED, NULL);

    ui_NewProfileScreen_name2 = lv_label_create(ui_NewProfileScreen_card2);
    lv_label_set_text(ui_NewProfileScreen_name2, LV_SYMBOL_DOWN);
    lv_obj_set_style_text_font(ui_NewProfileScreen_name2, &lv_font_montserrat_34, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewProfileScreen_name2, UI_COLOR_BG, LV_PART_MAIN);
    lv_obj_center(ui_NewProfileScreen_name2);

    // detail labels unused
    ui_NewProfileScreen_detail1 = NULL;
    ui_NewProfileScreen_detail2 = NULL;
}
