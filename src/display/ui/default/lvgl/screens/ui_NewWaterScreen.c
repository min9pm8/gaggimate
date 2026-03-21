#include "ui_NewWaterScreen.h"
#include "../ui_new_colors.h"
#include "../ui_new_events.h"

lv_obj_t *ui_NewWaterScreen = NULL;
lv_obj_t *ui_NewWaterScreen_tempArc = NULL;
lv_obj_t *ui_NewWaterScreen_targetLabel = NULL;
lv_obj_t *ui_NewWaterScreen_tempLabel = NULL;
lv_obj_t *ui_NewWaterScreen_statusLabel = NULL;
lv_obj_t *ui_NewWaterScreen_pumpBtn = NULL;
lv_obj_t *ui_NewWaterScreen_modeLabel = NULL;
lv_obj_t *ui_NewWaterScreen_dotsContainer = NULL;
lv_obj_t *ui_NewWaterScreen_dot1 = NULL;
lv_obj_t *ui_NewWaterScreen_dot2 = NULL;
lv_obj_t *ui_NewWaterScreen_dot3 = NULL;

void ui_NewWaterScreen_screen_init(void) {
    ui_NewWaterScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_NewWaterScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_NewWaterScreen, UI_COLOR_BG, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewWaterScreen, LV_OPA_COVER, LV_PART_MAIN);

    // --- Mode label above nav dots ---
    // --- Single arc (temperature) ---
    ui_NewWaterScreen_tempArc = lv_arc_create(ui_NewWaterScreen);
    lv_obj_set_size(ui_NewWaterScreen_tempArc, UI_RING_SINGLE_RADIUS * 2, UI_RING_SINGLE_RADIUS * 2);
    lv_obj_center(ui_NewWaterScreen_tempArc);
    lv_arc_set_rotation(ui_NewWaterScreen_tempArc, UI_ARC_START_ANGLE);
    lv_arc_set_bg_angles(ui_NewWaterScreen_tempArc, 0, UI_ARC_SWEEP_ANGLE);
    lv_arc_set_range(ui_NewWaterScreen_tempArc, UI_TEMP_WATER_MIN, UI_TEMP_WATER_MAX);
    lv_arc_set_value(ui_NewWaterScreen_tempArc, 0);
    lv_obj_remove_style(ui_NewWaterScreen_tempArc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(ui_NewWaterScreen_tempArc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_color(ui_NewWaterScreen_tempArc, UI_COLOR_SURFACE, LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui_NewWaterScreen_tempArc, UI_RING_SINGLE_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(ui_NewWaterScreen_tempArc, true, LV_PART_MAIN);
    lv_obj_set_style_arc_color(ui_NewWaterScreen_tempArc, UI_COLOR_BLUE, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(ui_NewWaterScreen_tempArc, UI_RING_SINGLE_WIDTH, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(ui_NewWaterScreen_tempArc, true, LV_PART_INDICATOR);

    // --- Center stack container ---
    lv_obj_t *center_stack = lv_obj_create(ui_NewWaterScreen);
    lv_obj_remove_style_all(center_stack);
    lv_obj_set_size(center_stack, 200, 200);
    lv_obj_center(center_stack);
    lv_obj_set_flex_flow(center_stack, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(center_stack, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(center_stack, 5, LV_PART_MAIN);

    // --- Target temp label ---
    ui_NewWaterScreen_targetLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_NewWaterScreen_targetLabel, "target 83\xC2\xB0");
    lv_obj_set_style_text_font(ui_NewWaterScreen_targetLabel, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewWaterScreen_targetLabel, UI_COLOR_TEXT_TER, LV_PART_MAIN);

    // --- Current temp label ---
    ui_NewWaterScreen_tempLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_NewWaterScreen_tempLabel, "0\xC2\xB0");
    lv_obj_set_style_text_font(ui_NewWaterScreen_tempLabel, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewWaterScreen_tempLabel, UI_COLOR_TEXT_PRI, LV_PART_MAIN);

    // --- Status label ---
    ui_NewWaterScreen_statusLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_NewWaterScreen_statusLabel, "heating...");
    lv_obj_set_style_text_font(ui_NewWaterScreen_statusLabel, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewWaterScreen_statusLabel, UI_COLOR_BLUE, LV_PART_MAIN);

    // --- Pump button (50px circle, blue) ---
    ui_NewWaterScreen_pumpBtn = lv_btn_create(center_stack);
    lv_obj_set_size(ui_NewWaterScreen_pumpBtn, UI_PUMP_BTN_SIZE, UI_PUMP_BTN_SIZE);
    lv_obj_set_style_radius(ui_NewWaterScreen_pumpBtn, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewWaterScreen_pumpBtn, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewWaterScreen_pumpBtn, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(ui_NewWaterScreen_pumpBtn, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_NewWaterScreen_pumpBtn, 16, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(ui_NewWaterScreen_pumpBtn, UI_OPA(25), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_NewWaterScreen_pumpBtn, 0, LV_PART_MAIN);
    lv_obj_t *pumpIcon = lv_label_create(ui_NewWaterScreen_pumpBtn);
    lv_label_set_text(pumpIcon, LV_SYMBOL_TINT);
    lv_obj_set_style_text_font(pumpIcon, &lv_font_montserrat_34, LV_PART_MAIN);
    lv_obj_set_style_text_color(pumpIcon, UI_COLOR_STANDBY_ICON_PRI, LV_PART_MAIN);
    lv_obj_center(pumpIcon);
    lv_obj_add_event_cb(ui_NewWaterScreen_pumpBtn, ui_event_NewWaterScreen_pump, LV_EVENT_CLICKED, NULL);

    // --- Navigation dots ---
    ui_NewWaterScreen_dotsContainer = lv_obj_create(ui_NewWaterScreen);
    lv_obj_t *dots = ui_NewWaterScreen_dotsContainer;
    lv_obj_remove_style_all(dots);
    lv_obj_set_size(dots, UI_NAV_DOT_SIZE * 3 + UI_NAV_DOT_GAP * 2, UI_NAV_DOT_SIZE);
    lv_obj_align(dots, LV_ALIGN_BOTTOM_MID, 0, -UI_NAV_DOT_BOTTOM);
    lv_obj_set_flex_flow(dots, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(dots, UI_NAV_DOT_GAP, LV_PART_MAIN);

    ui_NewWaterScreen_dot1 = lv_obj_create(dots);
    lv_obj_remove_style_all(ui_NewWaterScreen_dot1);
    lv_obj_set_size(ui_NewWaterScreen_dot1, UI_NAV_DOT_SIZE, UI_NAV_DOT_SIZE);
    lv_obj_set_style_radius(ui_NewWaterScreen_dot1, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewWaterScreen_dot1, UI_COLOR_MUTED, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewWaterScreen_dot1, LV_OPA_COVER, LV_PART_MAIN);

    ui_NewWaterScreen_dot2 = lv_obj_create(dots);
    lv_obj_remove_style_all(ui_NewWaterScreen_dot2);
    lv_obj_set_size(ui_NewWaterScreen_dot2, UI_NAV_DOT_SIZE, UI_NAV_DOT_SIZE);
    lv_obj_set_style_radius(ui_NewWaterScreen_dot2, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewWaterScreen_dot2, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewWaterScreen_dot2, LV_OPA_COVER, LV_PART_MAIN);

    ui_NewWaterScreen_dot3 = lv_obj_create(dots);
    lv_obj_remove_style_all(ui_NewWaterScreen_dot3);
    lv_obj_set_size(ui_NewWaterScreen_dot3, UI_NAV_DOT_SIZE, UI_NAV_DOT_SIZE);
    lv_obj_set_style_radius(ui_NewWaterScreen_dot3, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewWaterScreen_dot3, UI_COLOR_MUTED, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewWaterScreen_dot3, LV_OPA_COVER, LV_PART_MAIN);

    // --- Mode label above nav dots (created last so it's on top) ---
    ui_NewWaterScreen_modeLabel = lv_label_create(ui_NewWaterScreen);
    lv_label_set_text(ui_NewWaterScreen_modeLabel, "HOT WATER");
    lv_obj_set_style_text_font(ui_NewWaterScreen_modeLabel, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewWaterScreen_modeLabel, UI_COLOR_TEXT_SEC, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_NewWaterScreen_modeLabel, 2, LV_PART_MAIN);
    lv_obj_align(ui_NewWaterScreen_modeLabel, LV_ALIGN_BOTTOM_MID, 0, -30);
}
