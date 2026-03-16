#ifndef UI_NEW_WATER_SCREEN_H
#define UI_NEW_WATER_SCREEN_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t *ui_NewWaterScreen;

// Ring gauge
extern lv_obj_t *ui_NewWaterScreen_tempArc;

// Center stack
extern lv_obj_t *ui_NewWaterScreen_targetLabel;
extern lv_obj_t *ui_NewWaterScreen_tempLabel;
extern lv_obj_t *ui_NewWaterScreen_statusLabel;
extern lv_obj_t *ui_NewWaterScreen_pumpBtn;

// Mode label
extern lv_obj_t *ui_NewWaterScreen_modeLabel;

// Nav dots container
extern lv_obj_t *ui_NewWaterScreen_dotsContainer;

// Navigation dots
extern lv_obj_t *ui_NewWaterScreen_dot1;
extern lv_obj_t *ui_NewWaterScreen_dot2;
extern lv_obj_t *ui_NewWaterScreen_dot3;

void ui_NewWaterScreen_screen_init(void);

#ifdef __cplusplus
}
#endif

#endif
