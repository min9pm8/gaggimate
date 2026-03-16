#ifndef UI_NEW_STEAM_SCREEN_H
#define UI_NEW_STEAM_SCREEN_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t *ui_NewSteamScreen;

// Ring gauge
extern lv_obj_t *ui_NewSteamScreen_tempArc;

// Center stack
extern lv_obj_t *ui_NewSteamScreen_targetLabel;
extern lv_obj_t *ui_NewSteamScreen_tempLabel;
extern lv_obj_t *ui_NewSteamScreen_statusLabel;

// Mode label
extern lv_obj_t *ui_NewSteamScreen_modeLabel;

// Nav dots container
extern lv_obj_t *ui_NewSteamScreen_dotsContainer;

// Navigation dots
extern lv_obj_t *ui_NewSteamScreen_dot1;
extern lv_obj_t *ui_NewSteamScreen_dot2;
extern lv_obj_t *ui_NewSteamScreen_dot3;

void ui_NewSteamScreen_screen_init(void);

#ifdef __cplusplus
}
#endif

#endif
