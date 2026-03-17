#include "ui_NewBrewScreen.h"
#include "../ui_new_colors.h"
#include "../ui_new_events.h"

lv_obj_t *ui_NewBrewScreen = NULL;
lv_obj_t *ui_NewBrewScreen_pressureArc = NULL;
lv_obj_t *ui_NewBrewScreen_tempArc = NULL;
lv_obj_t *ui_NewBrewScreen_flushBtn = NULL;
lv_obj_t *ui_NewBrewScreen_flushIcon = NULL;
lv_obj_t *ui_NewBrewScreen_tempLabel = NULL;
lv_obj_t *ui_NewBrewScreen_pressureLabel = NULL;
lv_obj_t *ui_NewBrewScreen_brewBtn = NULL;
lv_obj_t *ui_NewBrewScreen_brewIcon = NULL;
lv_obj_t *ui_NewBrewScreen_timerLabel = NULL;
lv_obj_t *ui_NewBrewScreen_phaseLabel = NULL;
lv_obj_t *ui_NewBrewScreen_stopBtn = NULL;
lv_obj_t *ui_NewBrewScreen_stopIcon = NULL;
lv_obj_t *ui_NewBrewScreen_flushingLabel = NULL;
lv_obj_t *ui_NewBrewScreen_completeBtn = NULL;
lv_obj_t *ui_NewBrewScreen_dotsContainer = NULL;
lv_obj_t *ui_NewBrewScreen_modeLabel = NULL;
lv_obj_t *ui_NewBrewScreen_dot1 = NULL;
lv_obj_t *ui_NewBrewScreen_dot2 = NULL;
lv_obj_t *ui_NewBrewScreen_dot3 = NULL;

void ui_NewBrewScreen_screen_init(void) {
    ui_NewBrewScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_NewBrewScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_NewBrewScreen, UI_COLOR_BG, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewBrewScreen, LV_OPA_COVER, LV_PART_MAIN);

    // --- Outer arc (pressure) ---
    ui_NewBrewScreen_pressureArc = lv_arc_create(ui_NewBrewScreen);
    lv_obj_set_size(ui_NewBrewScreen_pressureArc, UI_RING_OUTER_RADIUS * 2, UI_RING_OUTER_RADIUS * 2);
    lv_obj_center(ui_NewBrewScreen_pressureArc);
    lv_arc_set_rotation(ui_NewBrewScreen_pressureArc, UI_ARC_START_ANGLE);
    lv_arc_set_bg_angles(ui_NewBrewScreen_pressureArc, 0, 270);
    lv_arc_set_range(ui_NewBrewScreen_pressureArc, UI_PRESSURE_MIN * 10, UI_PRESSURE_MAX * 10);
    lv_arc_set_value(ui_NewBrewScreen_pressureArc, 0);
    lv_obj_remove_style(ui_NewBrewScreen_pressureArc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(ui_NewBrewScreen_pressureArc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_color(ui_NewBrewScreen_pressureArc, UI_COLOR_SURFACE, LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui_NewBrewScreen_pressureArc, UI_RING_OUTER_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(ui_NewBrewScreen_pressureArc, true, LV_PART_MAIN);
    lv_obj_set_style_arc_color(ui_NewBrewScreen_pressureArc, UI_COLOR_GREEN, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(ui_NewBrewScreen_pressureArc, UI_RING_OUTER_WIDTH, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(ui_NewBrewScreen_pressureArc, true, LV_PART_INDICATOR);

    // --- Inner arc (temperature) ---
    ui_NewBrewScreen_tempArc = lv_arc_create(ui_NewBrewScreen);
    lv_obj_set_size(ui_NewBrewScreen_tempArc, UI_RING_INNER_RADIUS * 2, UI_RING_INNER_RADIUS * 2);
    lv_obj_center(ui_NewBrewScreen_tempArc);
    lv_arc_set_rotation(ui_NewBrewScreen_tempArc, UI_ARC_START_ANGLE);
    lv_arc_set_bg_angles(ui_NewBrewScreen_tempArc, 0, 270);
    lv_arc_set_range(ui_NewBrewScreen_tempArc, UI_TEMP_BREW_MIN, UI_TEMP_BREW_MAX);
    lv_arc_set_value(ui_NewBrewScreen_tempArc, 0);
    lv_obj_remove_style(ui_NewBrewScreen_tempArc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(ui_NewBrewScreen_tempArc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_color(ui_NewBrewScreen_tempArc, UI_COLOR_SURFACE, LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui_NewBrewScreen_tempArc, UI_RING_INNER_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(ui_NewBrewScreen_tempArc, true, LV_PART_MAIN);
    lv_obj_set_style_arc_color(ui_NewBrewScreen_tempArc, UI_COLOR_BLUE, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(ui_NewBrewScreen_tempArc, UI_RING_INNER_WIDTH, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(ui_NewBrewScreen_tempArc, true, LV_PART_INDICATOR);

    // --- Center stack container ---
    lv_obj_t *center_stack = lv_obj_create(ui_NewBrewScreen);
    lv_obj_remove_style_all(center_stack);
    lv_obj_set_size(center_stack, 200, 260);
    lv_obj_center(center_stack);
    lv_obj_set_flex_flow(center_stack, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(center_stack, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(center_stack, 5, LV_PART_MAIN);

    // --- Flush button (circle, 48px) ---
    ui_NewBrewScreen_flushBtn = lv_btn_create(center_stack);
    lv_obj_set_size(ui_NewBrewScreen_flushBtn, UI_FLUSH_BTN_SIZE, UI_FLUSH_BTN_SIZE);
    lv_obj_set_style_radius(ui_NewBrewScreen_flushBtn, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewBrewScreen_flushBtn, UI_COLOR_STANDBY_ICON_PRI, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewBrewScreen_flushBtn, UI_OPA(3), LV_PART_MAIN);
    lv_obj_set_style_border_color(ui_NewBrewScreen_flushBtn, UI_COLOR_BTN_BORDER, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_NewBrewScreen_flushBtn, 2, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_NewBrewScreen_flushBtn, 0, LV_PART_MAIN);
    ui_NewBrewScreen_flushIcon = lv_label_create(ui_NewBrewScreen_flushBtn);
    lv_label_set_text(ui_NewBrewScreen_flushIcon, LV_SYMBOL_TINT);
    lv_obj_set_style_text_font(ui_NewBrewScreen_flushIcon, &lv_font_montserrat_34, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewBrewScreen_flushIcon, UI_COLOR_TEXT_SEC, LV_PART_MAIN);
    lv_obj_center(ui_NewBrewScreen_flushIcon);
    lv_obj_add_event_cb(ui_NewBrewScreen_flushBtn, ui_event_NewBrewScreen_flush, LV_EVENT_CLICKED, NULL);

    // --- Flushing label (right after flush btn in flex order) ---
    ui_NewBrewScreen_flushingLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_NewBrewScreen_flushingLabel, "FLUSHING");
    lv_obj_set_style_text_font(ui_NewBrewScreen_flushingLabel, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewBrewScreen_flushingLabel, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_NewBrewScreen_flushingLabel, 2, LV_PART_MAIN);
    lv_obj_add_flag(ui_NewBrewScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);

    // --- Timer label (hidden in idle) ---
    ui_NewBrewScreen_timerLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_NewBrewScreen_timerLabel, "0:00");
    lv_obj_set_style_text_font(ui_NewBrewScreen_timerLabel, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewBrewScreen_timerLabel, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_NewBrewScreen_timerLabel, 1, LV_PART_MAIN);
    lv_obj_add_flag(ui_NewBrewScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);

    // --- Temperature label ---
    ui_NewBrewScreen_tempLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_NewBrewScreen_tempLabel, "0\xC2\xB0");
    lv_obj_set_style_text_font(ui_NewBrewScreen_tempLabel, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewBrewScreen_tempLabel, UI_COLOR_TEXT_PRI, LV_PART_MAIN);

    // --- Pressure label ---
    ui_NewBrewScreen_pressureLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_NewBrewScreen_pressureLabel, "0.0 bar");
    lv_obj_set_style_text_font(ui_NewBrewScreen_pressureLabel, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewBrewScreen_pressureLabel, UI_COLOR_GREEN, LV_PART_MAIN);

    // --- Brew button (circle, 52px, green) ---
    ui_NewBrewScreen_brewBtn = lv_btn_create(center_stack);
    lv_obj_set_size(ui_NewBrewScreen_brewBtn, UI_BREW_BTN_SIZE, UI_BREW_BTN_SIZE);
    lv_obj_set_style_radius(ui_NewBrewScreen_brewBtn, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewBrewScreen_brewBtn, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewBrewScreen_brewBtn, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(ui_NewBrewScreen_brewBtn, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_NewBrewScreen_brewBtn, 20, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(ui_NewBrewScreen_brewBtn, UI_OPA(20), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_NewBrewScreen_brewBtn, 0, LV_PART_MAIN);
    ui_NewBrewScreen_brewIcon = lv_label_create(ui_NewBrewScreen_brewBtn);
    lv_label_set_text(ui_NewBrewScreen_brewIcon, LV_SYMBOL_PLAY);
    lv_obj_set_style_text_font(ui_NewBrewScreen_brewIcon, &lv_font_montserrat_34, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewBrewScreen_brewIcon, UI_COLOR_BG, LV_PART_MAIN);
    lv_obj_center(ui_NewBrewScreen_brewIcon);
    lv_obj_add_event_cb(ui_NewBrewScreen_brewBtn, ui_event_NewBrewScreen_brew, LV_EVENT_CLICKED, NULL);

    // --- Phase label (hidden in idle) ---
    ui_NewBrewScreen_phaseLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_NewBrewScreen_phaseLabel, "");
    lv_obj_set_style_text_font(ui_NewBrewScreen_phaseLabel, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewBrewScreen_phaseLabel, UI_COLOR_TEXT_SEC, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_NewBrewScreen_phaseLabel, 1, LV_PART_MAIN);
    lv_obj_add_flag(ui_NewBrewScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);

    // --- Stop button (hidden in idle) ---
    ui_NewBrewScreen_stopBtn = lv_btn_create(center_stack);
    lv_obj_set_size(ui_NewBrewScreen_stopBtn, UI_STOP_BTN_SIZE, UI_STOP_BTN_SIZE);
    lv_obj_set_style_radius(ui_NewBrewScreen_stopBtn, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewBrewScreen_stopBtn, UI_COLOR_RED, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewBrewScreen_stopBtn, UI_OPA(15), LV_PART_MAIN);
    lv_obj_set_style_border_color(ui_NewBrewScreen_stopBtn, UI_COLOR_RED, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_NewBrewScreen_stopBtn, 2, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_NewBrewScreen_stopBtn, 0, LV_PART_MAIN);
    ui_NewBrewScreen_stopIcon = lv_label_create(ui_NewBrewScreen_stopBtn);
    lv_label_set_text(ui_NewBrewScreen_stopIcon, LV_SYMBOL_STOP);
    lv_obj_set_style_text_font(ui_NewBrewScreen_stopIcon, &lv_font_montserrat_34, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewBrewScreen_stopIcon, UI_COLOR_RED, LV_PART_MAIN);
    lv_obj_center(ui_NewBrewScreen_stopIcon);
    lv_obj_add_flag(ui_NewBrewScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(ui_NewBrewScreen_stopBtn, ui_event_NewBrewScreen_stop, LV_EVENT_CLICKED, NULL);

    // --- Complete dismiss button (hidden) ---
    ui_NewBrewScreen_completeBtn = lv_btn_create(center_stack);
    lv_obj_set_size(ui_NewBrewScreen_completeBtn, UI_STOP_BTN_SIZE, UI_STOP_BTN_SIZE);
    lv_obj_set_style_radius(ui_NewBrewScreen_completeBtn, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewBrewScreen_completeBtn, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewBrewScreen_completeBtn, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_NewBrewScreen_completeBtn, 0, LV_PART_MAIN);
    lv_obj_t *checkLabel = lv_label_create(ui_NewBrewScreen_completeBtn);
    lv_label_set_text(checkLabel, LV_SYMBOL_OK);
    lv_obj_set_style_text_font(checkLabel, &lv_font_montserrat_34, LV_PART_MAIN);
    lv_obj_set_style_text_color(checkLabel, UI_COLOR_BG, LV_PART_MAIN);
    lv_obj_center(checkLabel);
    lv_obj_add_flag(ui_NewBrewScreen_completeBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(ui_NewBrewScreen_completeBtn, ui_event_NewBrewScreen_complete_dismiss, LV_EVENT_CLICKED, NULL);

    // --- Navigation dots ---
    ui_NewBrewScreen_dotsContainer = lv_obj_create(ui_NewBrewScreen);
    lv_obj_t *dots = ui_NewBrewScreen_dotsContainer;
    lv_obj_remove_style_all(dots);
    lv_obj_set_size(dots, UI_NAV_DOT_SIZE * 3 + UI_NAV_DOT_GAP * 2, UI_NAV_DOT_SIZE);
    lv_obj_align(dots, LV_ALIGN_BOTTOM_MID, 0, -UI_NAV_DOT_BOTTOM);
    lv_obj_set_flex_flow(dots, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(dots, UI_NAV_DOT_GAP, LV_PART_MAIN);

    ui_NewBrewScreen_dot1 = lv_obj_create(dots);
    lv_obj_remove_style_all(ui_NewBrewScreen_dot1);
    lv_obj_set_size(ui_NewBrewScreen_dot1, UI_NAV_DOT_SIZE, UI_NAV_DOT_SIZE);
    lv_obj_set_style_radius(ui_NewBrewScreen_dot1, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewBrewScreen_dot1, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewBrewScreen_dot1, LV_OPA_COVER, LV_PART_MAIN);

    ui_NewBrewScreen_dot2 = lv_obj_create(dots);
    lv_obj_remove_style_all(ui_NewBrewScreen_dot2);
    lv_obj_set_size(ui_NewBrewScreen_dot2, UI_NAV_DOT_SIZE, UI_NAV_DOT_SIZE);
    lv_obj_set_style_radius(ui_NewBrewScreen_dot2, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewBrewScreen_dot2, UI_COLOR_MUTED, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewBrewScreen_dot2, LV_OPA_COVER, LV_PART_MAIN);

    ui_NewBrewScreen_dot3 = lv_obj_create(dots);
    lv_obj_remove_style_all(ui_NewBrewScreen_dot3);
    lv_obj_set_size(ui_NewBrewScreen_dot3, UI_NAV_DOT_SIZE, UI_NAV_DOT_SIZE);
    lv_obj_set_style_radius(ui_NewBrewScreen_dot3, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewBrewScreen_dot3, UI_COLOR_MUTED, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewBrewScreen_dot3, LV_OPA_COVER, LV_PART_MAIN);

    // --- Mode label above nav dots (created last so it's on top) ---
    ui_NewBrewScreen_modeLabel = lv_label_create(ui_NewBrewScreen);
    lv_label_set_text(ui_NewBrewScreen_modeLabel, "BREW");
    lv_obj_set_style_text_font(ui_NewBrewScreen_modeLabel, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewBrewScreen_modeLabel, UI_COLOR_TEXT_SEC, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_NewBrewScreen_modeLabel, 2, LV_PART_MAIN);
    lv_obj_align(ui_NewBrewScreen_modeLabel, LV_ALIGN_BOTTOM_MID, 0, -30);

    // Start in idle state
    ui_NewBrewScreen_set_idle();
}

// === State Management ===

void ui_NewBrewScreen_set_idle(void) {
    lv_obj_clear_flag(ui_NewBrewScreen_flushBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_tempLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_pressureLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_brewBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_dotsContainer, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_flag(ui_NewBrewScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_completeBtn, LV_OBJ_FLAG_HIDDEN);

    // Restore ring opacity (separate calls per part — LV_PART_MAIN | LV_PART_INDICATOR doesn't work)
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureArc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, LV_OPA_COVER, LV_PART_INDICATOR);

    // Restore text opacity
    lv_obj_set_style_opa(ui_NewBrewScreen_tempLabel, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureLabel, LV_OPA_COVER, LV_PART_MAIN);

    // Restore flush button idle style
    lv_obj_set_size(ui_NewBrewScreen_flushBtn, UI_FLUSH_BTN_SIZE, UI_FLUSH_BTN_SIZE);
    lv_obj_set_style_border_color(ui_NewBrewScreen_flushBtn, UI_COLOR_BTN_BORDER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewBrewScreen_flushBtn, UI_COLOR_STANDBY_ICON_PRI, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewBrewScreen_flushBtn, UI_OPA(3), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_NewBrewScreen_flushBtn, 0, LV_PART_MAIN);
    // Restore flush icon
    lv_label_set_text(ui_NewBrewScreen_flushIcon, LV_SYMBOL_TINT);
    lv_obj_set_style_text_color(ui_NewBrewScreen_flushIcon, UI_COLOR_TEXT_SEC, LV_PART_MAIN);

    // Restore timer color (may have been changed to white by complete state)
    lv_obj_set_style_text_color(ui_NewBrewScreen_timerLabel, UI_COLOR_GREEN, LV_PART_MAIN);
}

void ui_NewBrewScreen_set_flushing(void) {
    lv_obj_clear_flag(ui_NewBrewScreen_flushBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_tempLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_pressureLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_flag(ui_NewBrewScreen_brewBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_dotsContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_completeBtn, LV_OBJ_FLAG_HIDDEN);

    // Flush button active style
    lv_obj_set_size(ui_NewBrewScreen_flushBtn, UI_FLUSH_BTN_ACTIVE_SIZE, UI_FLUSH_BTN_ACTIVE_SIZE);
    lv_obj_set_style_border_color(ui_NewBrewScreen_flushBtn, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewBrewScreen_flushBtn, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewBrewScreen_flushBtn, UI_OPA(15), LV_PART_MAIN);
    lv_obj_set_style_shadow_color(ui_NewBrewScreen_flushBtn, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_NewBrewScreen_flushBtn, 24, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(ui_NewBrewScreen_flushBtn, UI_OPA(40), LV_PART_MAIN);
    lv_label_set_text(ui_NewBrewScreen_flushIcon, LV_SYMBOL_STOP);
    lv_obj_set_style_text_color(ui_NewBrewScreen_flushIcon, UI_COLOR_BLUE, LV_PART_MAIN);

    // Dim rings
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureArc, UI_OPA(30), LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureArc, UI_OPA(30), LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, UI_OPA(30), LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, UI_OPA(30), LV_PART_INDICATOR);

    // Dim text
    lv_obj_set_style_opa(ui_NewBrewScreen_tempLabel, UI_OPA(60), LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureLabel, UI_OPA(60), LV_PART_MAIN);
}

void ui_NewBrewScreen_set_brewing(void) {
    lv_obj_clear_flag(ui_NewBrewScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_tempLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_pressureLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_flag(ui_NewBrewScreen_flushBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_brewBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_dotsContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_completeBtn, LV_OBJ_FLAG_HIDDEN);

    // Restore ring and text opacity
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureArc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempLabel, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureLabel, LV_OPA_COVER, LV_PART_MAIN);
}

void ui_NewBrewScreen_set_complete(void) {
    lv_obj_clear_flag(ui_NewBrewScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_tempLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_pressureLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_completeBtn, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_flag(ui_NewBrewScreen_flushBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_brewBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_dotsContainer, LV_OBJ_FLAG_HIDDEN);

    // Timer turns white, phase shows COMPLETE
    lv_obj_set_style_text_color(ui_NewBrewScreen_timerLabel, UI_COLOR_STANDBY_ICON_PRI, LV_PART_MAIN);
    lv_label_set_text(ui_NewBrewScreen_phaseLabel, "COMPLETE");
}
