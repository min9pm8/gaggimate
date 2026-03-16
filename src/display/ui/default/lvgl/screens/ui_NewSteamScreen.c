#include "ui_NewSteamScreen.h"
#include "../ui_new_colors.h"
#include "../ui_new_events.h"

lv_obj_t *ui_NewSteamScreen = NULL;
lv_obj_t *ui_NewSteamScreen_tempArc = NULL;
lv_obj_t *ui_NewSteamScreen_targetLabel = NULL;
lv_obj_t *ui_NewSteamScreen_tempLabel = NULL;
lv_obj_t *ui_NewSteamScreen_statusLabel = NULL;
lv_obj_t *ui_NewSteamScreen_modeLabel = NULL;
lv_obj_t *ui_NewSteamScreen_dotsContainer = NULL;
lv_obj_t *ui_NewSteamScreen_dot1 = NULL;
lv_obj_t *ui_NewSteamScreen_dot2 = NULL;
lv_obj_t *ui_NewSteamScreen_dot3 = NULL;

void ui_NewSteamScreen_screen_init(void) {
    ui_NewSteamScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_NewSteamScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_NewSteamScreen, UI_COLOR_BG, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewSteamScreen, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(ui_NewSteamScreen, LV_RADIUS_CIRCLE, LV_PART_MAIN);

    // --- Mode label at top ---
    ui_NewSteamScreen_modeLabel = lv_label_create(ui_NewSteamScreen);
    lv_label_set_text(ui_NewSteamScreen_modeLabel, "STEAM");
    lv_obj_set_style_text_font(ui_NewSteamScreen_modeLabel, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewSteamScreen_modeLabel, UI_COLOR_TEXT_SEC, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_NewSteamScreen_modeLabel, 2, LV_PART_MAIN);
    lv_obj_align(ui_NewSteamScreen_modeLabel, LV_ALIGN_TOP_MID, 0, 40);

    // --- Single arc (temperature) ---
    ui_NewSteamScreen_tempArc = lv_arc_create(ui_NewSteamScreen);
    lv_obj_set_size(ui_NewSteamScreen_tempArc, UI_RING_SINGLE_RADIUS * 2, UI_RING_SINGLE_RADIUS * 2);
    lv_obj_center(ui_NewSteamScreen_tempArc);
    lv_arc_set_rotation(ui_NewSteamScreen_tempArc, UI_ARC_START_ANGLE);
    lv_arc_set_bg_angles(ui_NewSteamScreen_tempArc, 0, 270);
    lv_arc_set_range(ui_NewSteamScreen_tempArc, UI_TEMP_STEAM_MIN, UI_TEMP_STEAM_MAX);
    lv_arc_set_value(ui_NewSteamScreen_tempArc, 0);
    lv_obj_remove_style(ui_NewSteamScreen_tempArc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(ui_NewSteamScreen_tempArc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_color(ui_NewSteamScreen_tempArc, UI_COLOR_SURFACE, LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui_NewSteamScreen_tempArc, UI_RING_SINGLE_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(ui_NewSteamScreen_tempArc, true, LV_PART_MAIN);
    lv_obj_set_style_arc_color(ui_NewSteamScreen_tempArc, UI_COLOR_RED, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(ui_NewSteamScreen_tempArc, UI_RING_SINGLE_WIDTH, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(ui_NewSteamScreen_tempArc, true, LV_PART_INDICATOR);

    // --- Center stack container ---
    lv_obj_t *center_stack = lv_obj_create(ui_NewSteamScreen);
    lv_obj_remove_style_all(center_stack);
    lv_obj_set_size(center_stack, 120, 160);
    lv_obj_center(center_stack);
    lv_obj_set_flex_flow(center_stack, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(center_stack, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(center_stack, 5, LV_PART_MAIN);

    // --- Target temp label ---
    ui_NewSteamScreen_targetLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_NewSteamScreen_targetLabel, "target 143\xC2\xB0");
    lv_obj_set_style_text_font(ui_NewSteamScreen_targetLabel, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewSteamScreen_targetLabel, UI_COLOR_TEXT_TER, LV_PART_MAIN);

    // --- Current temp label ---
    ui_NewSteamScreen_tempLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_NewSteamScreen_tempLabel, "0\xC2\xB0");
    lv_obj_set_style_text_font(ui_NewSteamScreen_tempLabel, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewSteamScreen_tempLabel, UI_COLOR_TEXT_PRI, LV_PART_MAIN);

    // --- Status label ---
    ui_NewSteamScreen_statusLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_NewSteamScreen_statusLabel, "heating...");
    lv_obj_set_style_text_font(ui_NewSteamScreen_statusLabel, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewSteamScreen_statusLabel, UI_COLOR_RED, LV_PART_MAIN);

    // --- Navigation dots ---
    ui_NewSteamScreen_dotsContainer = lv_obj_create(ui_NewSteamScreen);
    lv_obj_t *dots = ui_NewSteamScreen_dotsContainer;
    lv_obj_remove_style_all(dots);
    lv_obj_set_size(dots, UI_NAV_DOT_SIZE * 3 + UI_NAV_DOT_GAP * 2, UI_NAV_DOT_SIZE);
    lv_obj_align(dots, LV_ALIGN_BOTTOM_MID, 0, -UI_NAV_DOT_BOTTOM);
    lv_obj_set_flex_flow(dots, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(dots, UI_NAV_DOT_GAP, LV_PART_MAIN);

    ui_NewSteamScreen_dot1 = lv_obj_create(dots);
    lv_obj_remove_style_all(ui_NewSteamScreen_dot1);
    lv_obj_set_size(ui_NewSteamScreen_dot1, UI_NAV_DOT_SIZE, UI_NAV_DOT_SIZE);
    lv_obj_set_style_radius(ui_NewSteamScreen_dot1, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewSteamScreen_dot1, UI_COLOR_MUTED, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewSteamScreen_dot1, LV_OPA_COVER, LV_PART_MAIN);

    ui_NewSteamScreen_dot2 = lv_obj_create(dots);
    lv_obj_remove_style_all(ui_NewSteamScreen_dot2);
    lv_obj_set_size(ui_NewSteamScreen_dot2, UI_NAV_DOT_SIZE, UI_NAV_DOT_SIZE);
    lv_obj_set_style_radius(ui_NewSteamScreen_dot2, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewSteamScreen_dot2, UI_COLOR_MUTED, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewSteamScreen_dot2, LV_OPA_COVER, LV_PART_MAIN);

    ui_NewSteamScreen_dot3 = lv_obj_create(dots);
    lv_obj_remove_style_all(ui_NewSteamScreen_dot3);
    lv_obj_set_size(ui_NewSteamScreen_dot3, UI_NAV_DOT_SIZE, UI_NAV_DOT_SIZE);
    lv_obj_set_style_radius(ui_NewSteamScreen_dot3, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewSteamScreen_dot3, UI_COLOR_RED, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewSteamScreen_dot3, LV_OPA_COVER, LV_PART_MAIN);

    // --- Gesture handler ---
    lv_obj_add_event_cb(ui_NewSteamScreen, ui_event_NewSteamScreen_gesture, LV_EVENT_GESTURE, NULL);
}
