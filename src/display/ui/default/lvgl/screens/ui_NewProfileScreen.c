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

static lv_obj_t *create_profile_card(lv_obj_t *parent,
                                      lv_obj_t **card_out,
                                      lv_obj_t **name_out,
                                      lv_obj_t **detail_out,
                                      lv_event_cb_t tap_cb) {
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_size(card, 280, 80);
    lv_obj_set_style_bg_color(card, UI_COLOR_SURFACE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(card, UI_COLOR_BTN_BORDER, LV_PART_MAIN);
    lv_obj_set_style_border_width(card, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(card, 16, LV_PART_MAIN);
    lv_obj_set_style_outline_width(card, 0, LV_PART_MAIN);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_add_flag(card, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(card, tap_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *name = lv_label_create(card);
    lv_label_set_text(name, "Profile");
    lv_obj_set_style_text_font(name, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(name, UI_COLOR_TEXT_PRI, LV_PART_MAIN);

    lv_obj_t *detail = lv_label_create(card);
    lv_label_set_text(detail, "");
    lv_obj_set_style_text_font(detail, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(detail, UI_COLOR_TEXT_SEC, LV_PART_MAIN);

    if (card_out) *card_out = card;
    if (name_out) *name_out = name;
    if (detail_out) *detail_out = detail;

    return card;
}

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

    // --- Centered column container ---
    lv_obj_t *col = lv_obj_create(ui_NewProfileScreen);
    lv_obj_remove_style_all(col);
    lv_obj_set_size(col, 320, 280);
    lv_obj_center(col);
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(col, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(col, 16, LV_PART_MAIN);

    // --- Title ---
    lv_obj_t *title = lv_label_create(col);
    lv_label_set_text(title, "SELECT PROFILE");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(title, UI_COLOR_TEXT_SEC, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(title, 2, LV_PART_MAIN);

    // --- Card 1 ---
    create_profile_card(col,
                        &ui_NewProfileScreen_card1,
                        &ui_NewProfileScreen_name1,
                        &ui_NewProfileScreen_detail1,
                        ui_event_NewProfileScreen_card1);

    // --- Card 2 ---
    create_profile_card(col,
                        &ui_NewProfileScreen_card2,
                        &ui_NewProfileScreen_name2,
                        &ui_NewProfileScreen_detail2,
                        ui_event_NewProfileScreen_card2);
}
