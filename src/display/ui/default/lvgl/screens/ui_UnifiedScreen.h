#ifndef UI_UNIFIED_SCREEN_H
#define UI_UNIFIED_SCREEN_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t *ui_UnifiedScreen;

// Ring gauges
extern lv_obj_t *ui_UnifiedScreen_outerArc;
extern lv_obj_t *ui_UnifiedScreen_innerArc;

// Center stack
extern lv_obj_t *ui_UnifiedScreen_flushBtn;
extern lv_obj_t *ui_UnifiedScreen_flushIcon;
extern lv_obj_t *ui_UnifiedScreen_tempLabel;
extern lv_obj_t *ui_UnifiedScreen_pressureLabel;
extern lv_obj_t *ui_UnifiedScreen_actionBtn;
extern lv_obj_t *ui_UnifiedScreen_actionIcon;
extern lv_obj_t *ui_UnifiedScreen_stopBtn;
extern lv_obj_t *ui_UnifiedScreen_stopIcon;

// Flushing state
extern lv_obj_t *ui_UnifiedScreen_flushingLabel;

// Brew active state
extern lv_obj_t *ui_UnifiedScreen_timerLabel;
extern lv_obj_t *ui_UnifiedScreen_phaseLabel;

// Complete state
extern lv_obj_t *ui_UnifiedScreen_completeBtn;


void ui_UnifiedScreen_screen_init(void);

// State management
void ui_UnifiedScreen_set_idle(void);
void ui_UnifiedScreen_set_flushing(void);
void ui_UnifiedScreen_set_brewing(void);
void ui_UnifiedScreen_set_complete(void);

// Mode switching — updates ring colors, button visibility, arc ranges
void ui_UnifiedScreen_set_mode_brew(void);
void ui_UnifiedScreen_set_mode_water(void);
void ui_UnifiedScreen_set_mode_steam(void);

#ifdef __cplusplus
}
#endif

#endif
