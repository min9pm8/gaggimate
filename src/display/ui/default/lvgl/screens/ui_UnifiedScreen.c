#include "ui_UnifiedScreen.h"
#include "../ui_new_colors.h"
#include "../ui_new_events.h"

lv_obj_t *ui_UnifiedScreen = NULL;
lv_obj_t *ui_UnifiedScreen_outerArc = NULL;
lv_obj_t *ui_UnifiedScreen_innerArc = NULL;
lv_obj_t *ui_UnifiedScreen_flushBtn = NULL;
lv_obj_t *ui_UnifiedScreen_flushIcon = NULL;
lv_obj_t *ui_UnifiedScreen_tempLabel = NULL;
lv_obj_t *ui_UnifiedScreen_pressureLabel = NULL;
lv_obj_t *ui_UnifiedScreen_actionBtn = NULL;
lv_obj_t *ui_UnifiedScreen_actionIcon = NULL;
lv_obj_t *ui_UnifiedScreen_stopBtn = NULL;
lv_obj_t *ui_UnifiedScreen_stopIcon = NULL;
lv_obj_t *ui_UnifiedScreen_flushingLabel = NULL;
lv_obj_t *ui_UnifiedScreen_timerLabel = NULL;
lv_obj_t *ui_UnifiedScreen_phaseLabel = NULL;
lv_obj_t *ui_UnifiedScreen_completeBtn = NULL;
lv_obj_t *ui_UnifiedScreen_standbyBtn = NULL;
lv_obj_t *ui_UnifiedScreen_leftZone = NULL;
lv_obj_t *ui_UnifiedScreen_rightZone = NULL;

void ui_UnifiedScreen_screen_init(void) {
    ui_UnifiedScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_UnifiedScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_UnifiedScreen, UI_COLOR_BG, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_UnifiedScreen, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_UnifiedScreen, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(ui_UnifiedScreen, 0, LV_PART_MAIN);

    // --- Outer arc (pressure in brew, temp in water/steam) ---
    ui_UnifiedScreen_outerArc = lv_arc_create(ui_UnifiedScreen);
    lv_obj_set_size(ui_UnifiedScreen_outerArc, UI_RING_OUTER_RADIUS * 2, UI_RING_OUTER_RADIUS * 2);
    lv_obj_center(ui_UnifiedScreen_outerArc);
    lv_arc_set_rotation(ui_UnifiedScreen_outerArc, UI_ARC_START_ANGLE);
    lv_arc_set_bg_angles(ui_UnifiedScreen_outerArc, 0, 270);
    lv_arc_set_range(ui_UnifiedScreen_outerArc, UI_PRESSURE_MIN * 10, UI_PRESSURE_MAX * 10);
    lv_arc_set_value(ui_UnifiedScreen_outerArc, 0);
    lv_obj_remove_style(ui_UnifiedScreen_outerArc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(ui_UnifiedScreen_outerArc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_color(ui_UnifiedScreen_outerArc, UI_COLOR_SURFACE, LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui_UnifiedScreen_outerArc, UI_RING_OUTER_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(ui_UnifiedScreen_outerArc, true, LV_PART_MAIN);
    lv_obj_set_style_arc_color(ui_UnifiedScreen_outerArc, UI_COLOR_GREEN, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(ui_UnifiedScreen_outerArc, UI_RING_OUTER_WIDTH, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(ui_UnifiedScreen_outerArc, true, LV_PART_INDICATOR);

    // --- Inner arc (temperature in brew mode, hidden otherwise) ---
    ui_UnifiedScreen_innerArc = lv_arc_create(ui_UnifiedScreen);
    lv_obj_set_size(ui_UnifiedScreen_innerArc, UI_RING_INNER_RADIUS * 2, UI_RING_INNER_RADIUS * 2);
    lv_obj_center(ui_UnifiedScreen_innerArc);
    lv_arc_set_rotation(ui_UnifiedScreen_innerArc, UI_ARC_START_ANGLE);
    lv_arc_set_bg_angles(ui_UnifiedScreen_innerArc, 0, 270);
    lv_arc_set_range(ui_UnifiedScreen_innerArc, UI_TEMP_BREW_MIN, UI_TEMP_BREW_MAX);
    lv_arc_set_value(ui_UnifiedScreen_innerArc, 0);
    lv_obj_remove_style(ui_UnifiedScreen_innerArc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(ui_UnifiedScreen_innerArc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_color(ui_UnifiedScreen_innerArc, UI_COLOR_SURFACE, LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui_UnifiedScreen_innerArc, UI_RING_INNER_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(ui_UnifiedScreen_innerArc, true, LV_PART_MAIN);
    lv_obj_set_style_arc_color(ui_UnifiedScreen_innerArc, UI_COLOR_BLUE, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(ui_UnifiedScreen_innerArc, UI_RING_INNER_WIDTH, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(ui_UnifiedScreen_innerArc, true, LV_PART_INDICATOR);

    // --- Left tap zone (left 25% of screen) ---
    ui_UnifiedScreen_leftZone = lv_obj_create(ui_UnifiedScreen);
    lv_obj_remove_style_all(ui_UnifiedScreen_leftZone);
    lv_obj_set_size(ui_UnifiedScreen_leftZone, 116, 466);
    lv_obj_align(ui_UnifiedScreen_leftZone, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_opa(ui_UnifiedScreen_leftZone, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_add_flag(ui_UnifiedScreen_leftZone, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui_UnifiedScreen_leftZone, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(ui_UnifiedScreen_leftZone, ui_event_UnifiedScreen_left, LV_EVENT_CLICKED, NULL);

    // --- Right tap zone (right 25% of screen) ---
    ui_UnifiedScreen_rightZone = lv_obj_create(ui_UnifiedScreen);
    lv_obj_remove_style_all(ui_UnifiedScreen_rightZone);
    lv_obj_set_size(ui_UnifiedScreen_rightZone, 116, 466);
    lv_obj_align(ui_UnifiedScreen_rightZone, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_opa(ui_UnifiedScreen_rightZone, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_add_flag(ui_UnifiedScreen_rightZone, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui_UnifiedScreen_rightZone, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(ui_UnifiedScreen_rightZone, ui_event_UnifiedScreen_right, LV_EVENT_CLICKED, NULL);

    // --- Center stack container ---
    lv_obj_t *center_stack = lv_obj_create(ui_UnifiedScreen);
    lv_obj_remove_style_all(center_stack);
    lv_obj_set_size(center_stack, 200, 340);
    lv_obj_center(center_stack);
    lv_obj_set_flex_flow(center_stack, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(center_stack, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(center_stack, 5, LV_PART_MAIN);

    // --- Flush button (84px circle) ---
    ui_UnifiedScreen_flushBtn = lv_btn_create(center_stack);
    lv_obj_set_size(ui_UnifiedScreen_flushBtn, UI_FLUSH_BTN_SIZE, UI_FLUSH_BTN_SIZE);
    lv_obj_set_style_radius(ui_UnifiedScreen_flushBtn, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_UnifiedScreen_flushBtn, UI_COLOR_STANDBY_ICON_PRI, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_UnifiedScreen_flushBtn, UI_OPA(3), LV_PART_MAIN);
    lv_obj_set_style_border_color(ui_UnifiedScreen_flushBtn, UI_COLOR_BTN_BORDER, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_UnifiedScreen_flushBtn, 2, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_UnifiedScreen_flushBtn, 0, LV_PART_MAIN);
    ui_UnifiedScreen_flushIcon = lv_label_create(ui_UnifiedScreen_flushBtn);
    lv_label_set_text(ui_UnifiedScreen_flushIcon, LV_SYMBOL_TINT);
    lv_obj_set_style_text_font(ui_UnifiedScreen_flushIcon, &lv_font_montserrat_34, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_UnifiedScreen_flushIcon, UI_COLOR_TEXT_SEC, LV_PART_MAIN);
    lv_obj_center(ui_UnifiedScreen_flushIcon);
    lv_obj_add_event_cb(ui_UnifiedScreen_flushBtn, ui_event_UnifiedScreen_flush, LV_EVENT_CLICKED, NULL);

    // --- Flushing label (hidden by default) ---
    ui_UnifiedScreen_flushingLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_UnifiedScreen_flushingLabel, "FLUSHING");
    lv_obj_set_style_text_font(ui_UnifiedScreen_flushingLabel, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_UnifiedScreen_flushingLabel, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_UnifiedScreen_flushingLabel, 2, LV_PART_MAIN);
    lv_obj_add_flag(ui_UnifiedScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);

    // --- Timer label (hidden in idle) ---
    ui_UnifiedScreen_timerLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_UnifiedScreen_timerLabel, "0:00");
    lv_obj_set_style_text_font(ui_UnifiedScreen_timerLabel, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_UnifiedScreen_timerLabel, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_UnifiedScreen_timerLabel, 1, LV_PART_MAIN);
    lv_obj_add_flag(ui_UnifiedScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);

    // --- Temperature label ---
    ui_UnifiedScreen_tempLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_UnifiedScreen_tempLabel, "0\xC2\xB0");
    lv_obj_set_style_text_font(ui_UnifiedScreen_tempLabel, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_UnifiedScreen_tempLabel, UI_COLOR_TEXT_PRI, LV_PART_MAIN);

    // --- Pressure label ---
    ui_UnifiedScreen_pressureLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_UnifiedScreen_pressureLabel, "0.0 bar");
    lv_obj_set_style_text_font(ui_UnifiedScreen_pressureLabel, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_UnifiedScreen_pressureLabel, UI_COLOR_GREEN, LV_PART_MAIN);

    // --- Phase label (hidden in idle) ---
    ui_UnifiedScreen_phaseLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_UnifiedScreen_phaseLabel, "");
    lv_obj_set_style_text_font(ui_UnifiedScreen_phaseLabel, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_UnifiedScreen_phaseLabel, UI_COLOR_TEXT_SEC, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_UnifiedScreen_phaseLabel, 1, LV_PART_MAIN);
    lv_obj_add_flag(ui_UnifiedScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);

    // --- Action button (90px circle, green play) ---
    ui_UnifiedScreen_actionBtn = lv_btn_create(center_stack);
    lv_obj_set_size(ui_UnifiedScreen_actionBtn, UI_BREW_BTN_SIZE, UI_BREW_BTN_SIZE);
    lv_obj_set_style_radius(ui_UnifiedScreen_actionBtn, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_UnifiedScreen_actionBtn, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_UnifiedScreen_actionBtn, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(ui_UnifiedScreen_actionBtn, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_UnifiedScreen_actionBtn, 20, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(ui_UnifiedScreen_actionBtn, UI_OPA(20), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_UnifiedScreen_actionBtn, 0, LV_PART_MAIN);
    ui_UnifiedScreen_actionIcon = lv_label_create(ui_UnifiedScreen_actionBtn);
    lv_label_set_text(ui_UnifiedScreen_actionIcon, LV_SYMBOL_PLAY);
    lv_obj_set_style_text_font(ui_UnifiedScreen_actionIcon, &lv_font_montserrat_34, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_UnifiedScreen_actionIcon, UI_COLOR_BG, LV_PART_MAIN);
    lv_obj_center(ui_UnifiedScreen_actionIcon);
    lv_obj_add_event_cb(ui_UnifiedScreen_actionBtn, ui_event_UnifiedScreen_action, LV_EVENT_CLICKED, NULL);

    // --- Stop button (hidden in idle) ---
    ui_UnifiedScreen_stopBtn = lv_btn_create(center_stack);
    lv_obj_set_size(ui_UnifiedScreen_stopBtn, UI_STOP_BTN_SIZE, UI_STOP_BTN_SIZE);
    lv_obj_set_style_radius(ui_UnifiedScreen_stopBtn, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_UnifiedScreen_stopBtn, UI_COLOR_RED, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_UnifiedScreen_stopBtn, UI_OPA(15), LV_PART_MAIN);
    lv_obj_set_style_border_color(ui_UnifiedScreen_stopBtn, UI_COLOR_RED, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_UnifiedScreen_stopBtn, 2, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_UnifiedScreen_stopBtn, 0, LV_PART_MAIN);
    ui_UnifiedScreen_stopIcon = lv_label_create(ui_UnifiedScreen_stopBtn);
    lv_label_set_text(ui_UnifiedScreen_stopIcon, LV_SYMBOL_STOP);
    lv_obj_set_style_text_font(ui_UnifiedScreen_stopIcon, &lv_font_montserrat_34, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_UnifiedScreen_stopIcon, UI_COLOR_RED, LV_PART_MAIN);
    lv_obj_center(ui_UnifiedScreen_stopIcon);
    lv_obj_add_flag(ui_UnifiedScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(ui_UnifiedScreen_stopBtn, ui_event_UnifiedScreen_stop, LV_EVENT_CLICKED, NULL);

    // --- Complete dismiss button (hidden) ---
    ui_UnifiedScreen_completeBtn = lv_btn_create(center_stack);
    lv_obj_set_size(ui_UnifiedScreen_completeBtn, UI_STOP_BTN_SIZE, UI_STOP_BTN_SIZE);
    lv_obj_set_style_radius(ui_UnifiedScreen_completeBtn, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_UnifiedScreen_completeBtn, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_UnifiedScreen_completeBtn, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_UnifiedScreen_completeBtn, 0, LV_PART_MAIN);
    lv_obj_t *checkLabel = lv_label_create(ui_UnifiedScreen_completeBtn);
    lv_label_set_text(checkLabel, LV_SYMBOL_OK);
    lv_obj_set_style_text_font(checkLabel, &lv_font_montserrat_34, LV_PART_MAIN);
    lv_obj_set_style_text_color(checkLabel, UI_COLOR_BG, LV_PART_MAIN);
    lv_obj_center(checkLabel);
    lv_obj_add_flag(ui_UnifiedScreen_completeBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(ui_UnifiedScreen_completeBtn, ui_event_UnifiedScreen_complete, LV_EVENT_CLICKED, NULL);

    // --- Standby button (small, bottom center) ---
    ui_UnifiedScreen_standbyBtn = lv_btn_create(ui_UnifiedScreen);
    lv_obj_set_size(ui_UnifiedScreen_standbyBtn, 40, 40);
    lv_obj_set_style_radius(ui_UnifiedScreen_standbyBtn, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_UnifiedScreen_standbyBtn, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_color(ui_UnifiedScreen_standbyBtn, UI_COLOR_BTN_BORDER, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_UnifiedScreen_standbyBtn, 1, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_UnifiedScreen_standbyBtn, 0, LV_PART_MAIN);
    lv_obj_align(ui_UnifiedScreen_standbyBtn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_t *powerIcon = lv_label_create(ui_UnifiedScreen_standbyBtn);
    lv_label_set_text(powerIcon, LV_SYMBOL_POWER);
    lv_obj_set_style_text_font(powerIcon, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(powerIcon, UI_COLOR_TEXT_SEC, LV_PART_MAIN);
    lv_obj_center(powerIcon);
    lv_obj_add_event_cb(ui_UnifiedScreen_standbyBtn, ui_event_UnifiedScreen_standby, LV_EVENT_CLICKED, NULL);

    // Start in idle brew mode
    ui_UnifiedScreen_set_idle();
}

// === State Management ===

void ui_UnifiedScreen_set_idle(void) {
    lv_obj_clear_flag(ui_UnifiedScreen_flushBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_tempLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_pressureLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_actionBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_standbyBtn, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_flag(ui_UnifiedScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_completeBtn, LV_OBJ_FLAG_HIDDEN);

    // Restore ring opacity
    lv_obj_set_style_opa(ui_UnifiedScreen_outerArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_UnifiedScreen_outerArc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_UnifiedScreen_innerArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_UnifiedScreen_innerArc, LV_OPA_COVER, LV_PART_INDICATOR);

    // Restore text opacity
    lv_obj_set_style_opa(ui_UnifiedScreen_tempLabel, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_UnifiedScreen_pressureLabel, LV_OPA_COVER, LV_PART_MAIN);

    // Restore flush button idle style
    lv_obj_set_size(ui_UnifiedScreen_flushBtn, UI_FLUSH_BTN_SIZE, UI_FLUSH_BTN_SIZE);
    lv_obj_set_style_border_color(ui_UnifiedScreen_flushBtn, UI_COLOR_BTN_BORDER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_UnifiedScreen_flushBtn, UI_COLOR_STANDBY_ICON_PRI, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_UnifiedScreen_flushBtn, UI_OPA(3), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_UnifiedScreen_flushBtn, 0, LV_PART_MAIN);
    lv_label_set_text(ui_UnifiedScreen_flushIcon, LV_SYMBOL_TINT);
    lv_obj_set_style_text_color(ui_UnifiedScreen_flushIcon, UI_COLOR_TEXT_SEC, LV_PART_MAIN);

    // Restore timer color
    lv_obj_set_style_text_color(ui_UnifiedScreen_timerLabel, UI_COLOR_GREEN, LV_PART_MAIN);
}

void ui_UnifiedScreen_set_flushing(void) {
    lv_obj_clear_flag(ui_UnifiedScreen_flushBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_tempLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_pressureLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_flag(ui_UnifiedScreen_actionBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_standbyBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_completeBtn, LV_OBJ_FLAG_HIDDEN);

    // Flush button active style
    lv_obj_set_size(ui_UnifiedScreen_flushBtn, UI_FLUSH_BTN_ACTIVE_SIZE, UI_FLUSH_BTN_ACTIVE_SIZE);
    lv_obj_set_style_border_color(ui_UnifiedScreen_flushBtn, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_UnifiedScreen_flushBtn, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_UnifiedScreen_flushBtn, UI_OPA(15), LV_PART_MAIN);
    lv_obj_set_style_shadow_color(ui_UnifiedScreen_flushBtn, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_UnifiedScreen_flushBtn, 24, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(ui_UnifiedScreen_flushBtn, UI_OPA(40), LV_PART_MAIN);
    lv_label_set_text(ui_UnifiedScreen_flushIcon, LV_SYMBOL_STOP);
    lv_obj_set_style_text_color(ui_UnifiedScreen_flushIcon, UI_COLOR_BLUE, LV_PART_MAIN);

    // Dim rings
    lv_obj_set_style_opa(ui_UnifiedScreen_outerArc, UI_OPA(30), LV_PART_MAIN);
    lv_obj_set_style_opa(ui_UnifiedScreen_outerArc, UI_OPA(30), LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_UnifiedScreen_innerArc, UI_OPA(30), LV_PART_MAIN);
    lv_obj_set_style_opa(ui_UnifiedScreen_innerArc, UI_OPA(30), LV_PART_INDICATOR);

    // Dim text
    lv_obj_set_style_opa(ui_UnifiedScreen_tempLabel, UI_OPA(60), LV_PART_MAIN);
    lv_obj_set_style_opa(ui_UnifiedScreen_pressureLabel, UI_OPA(60), LV_PART_MAIN);
}

void ui_UnifiedScreen_set_brewing(void) {
    lv_obj_clear_flag(ui_UnifiedScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_tempLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_pressureLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_flag(ui_UnifiedScreen_flushBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_actionBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_standbyBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_completeBtn, LV_OBJ_FLAG_HIDDEN);

    // Restore ring and text opacity
    lv_obj_set_style_opa(ui_UnifiedScreen_outerArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_UnifiedScreen_outerArc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_UnifiedScreen_innerArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_UnifiedScreen_innerArc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_UnifiedScreen_tempLabel, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_UnifiedScreen_pressureLabel, LV_OPA_COVER, LV_PART_MAIN);
}

void ui_UnifiedScreen_set_complete(void) {
    lv_obj_clear_flag(ui_UnifiedScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_tempLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_pressureLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_completeBtn, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_flag(ui_UnifiedScreen_flushBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_actionBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_standbyBtn, LV_OBJ_FLAG_HIDDEN);

    // Timer turns white, phase shows COMPLETE
    lv_obj_set_style_text_color(ui_UnifiedScreen_timerLabel, UI_COLOR_STANDBY_ICON_PRI, LV_PART_MAIN);
    lv_label_set_text(ui_UnifiedScreen_phaseLabel, "COMPLETE");
}

// === Mode Switching ===

void ui_UnifiedScreen_set_mode_brew(void) {
    // Outer arc = green (pressure)
    lv_obj_set_style_arc_color(ui_UnifiedScreen_outerArc, UI_COLOR_GREEN, LV_PART_INDICATOR);
    lv_arc_set_range(ui_UnifiedScreen_outerArc, UI_PRESSURE_MIN * 10, UI_PRESSURE_MAX * 10);
    lv_arc_set_value(ui_UnifiedScreen_outerArc, 0);

    // Inner arc = blue (temp), visible
    lv_obj_clear_flag(ui_UnifiedScreen_innerArc, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_arc_color(ui_UnifiedScreen_innerArc, UI_COLOR_BLUE, LV_PART_INDICATOR);
    lv_arc_set_range(ui_UnifiedScreen_innerArc, UI_TEMP_BREW_MIN, UI_TEMP_BREW_MAX);
    lv_arc_set_value(ui_UnifiedScreen_innerArc, 0);

    // Action btn = green play
    lv_obj_clear_flag(ui_UnifiedScreen_actionBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_bg_color(ui_UnifiedScreen_actionBtn, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(ui_UnifiedScreen_actionBtn, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_label_set_text(ui_UnifiedScreen_actionIcon, LV_SYMBOL_PLAY);
    lv_obj_set_style_text_color(ui_UnifiedScreen_actionIcon, UI_COLOR_BG, LV_PART_MAIN);

    // Pressure visible, flush visible
    lv_obj_clear_flag(ui_UnifiedScreen_pressureLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_UnifiedScreen_flushBtn, LV_OBJ_FLAG_HIDDEN);

    ui_UnifiedScreen_set_idle();
}

void ui_UnifiedScreen_set_mode_water(void) {
    // Outer arc = blue (temp)
    lv_obj_set_style_arc_color(ui_UnifiedScreen_outerArc, UI_COLOR_BLUE, LV_PART_INDICATOR);
    lv_arc_set_range(ui_UnifiedScreen_outerArc, UI_TEMP_WATER_MIN, UI_TEMP_WATER_MAX);
    lv_arc_set_value(ui_UnifiedScreen_outerArc, 0);

    // Inner arc hidden
    lv_obj_add_flag(ui_UnifiedScreen_innerArc, LV_OBJ_FLAG_HIDDEN);

    // Action btn = blue droplet
    lv_obj_clear_flag(ui_UnifiedScreen_actionBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_bg_color(ui_UnifiedScreen_actionBtn, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(ui_UnifiedScreen_actionBtn, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_label_set_text(ui_UnifiedScreen_actionIcon, LV_SYMBOL_TINT);
    lv_obj_set_style_text_color(ui_UnifiedScreen_actionIcon, UI_COLOR_STANDBY_ICON_PRI, LV_PART_MAIN);

    // Pressure hidden, flush hidden
    lv_obj_add_flag(ui_UnifiedScreen_pressureLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_flushBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);

    // Show standby, hide brew-specific
    lv_obj_clear_flag(ui_UnifiedScreen_standbyBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_completeBtn, LV_OBJ_FLAG_HIDDEN);

    // Restore opacity
    lv_obj_set_style_opa(ui_UnifiedScreen_outerArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_UnifiedScreen_outerArc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_UnifiedScreen_tempLabel, LV_OPA_COVER, LV_PART_MAIN);
}

void ui_UnifiedScreen_set_mode_steam(void) {
    // Outer arc = red (temp), turns green when ready
    lv_obj_set_style_arc_color(ui_UnifiedScreen_outerArc, UI_COLOR_RED, LV_PART_INDICATOR);
    lv_arc_set_range(ui_UnifiedScreen_outerArc, UI_TEMP_STEAM_MIN, UI_TEMP_STEAM_MAX);
    lv_arc_set_value(ui_UnifiedScreen_outerArc, 0);

    // Inner arc hidden
    lv_obj_add_flag(ui_UnifiedScreen_innerArc, LV_OBJ_FLAG_HIDDEN);

    // Action btn hidden
    lv_obj_add_flag(ui_UnifiedScreen_actionBtn, LV_OBJ_FLAG_HIDDEN);

    // Pressure hidden, flush hidden
    lv_obj_add_flag(ui_UnifiedScreen_pressureLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_flushBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);

    // Show standby, hide brew-specific
    lv_obj_clear_flag(ui_UnifiedScreen_standbyBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UnifiedScreen_completeBtn, LV_OBJ_FLAG_HIDDEN);

    // Restore opacity
    lv_obj_set_style_opa(ui_UnifiedScreen_outerArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_UnifiedScreen_outerArc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_UnifiedScreen_tempLabel, LV_OPA_COVER, LV_PART_MAIN);
}
