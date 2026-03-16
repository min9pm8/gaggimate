#ifndef UI_NEW_BREW_SCREEN_H
#define UI_NEW_BREW_SCREEN_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t *ui_NewBrewScreen;

// Ring gauges
extern lv_obj_t *ui_NewBrewScreen_pressureArc;
extern lv_obj_t *ui_NewBrewScreen_tempArc;

// Center stack
extern lv_obj_t *ui_NewBrewScreen_flushBtn;
extern lv_obj_t *ui_NewBrewScreen_flushIcon;
extern lv_obj_t *ui_NewBrewScreen_tempLabel;
extern lv_obj_t *ui_NewBrewScreen_pressureLabel;
extern lv_obj_t *ui_NewBrewScreen_brewBtn;
extern lv_obj_t *ui_NewBrewScreen_brewIcon;

// Brew active state elements
extern lv_obj_t *ui_NewBrewScreen_timerLabel;
extern lv_obj_t *ui_NewBrewScreen_phaseLabel;
extern lv_obj_t *ui_NewBrewScreen_stopBtn;
extern lv_obj_t *ui_NewBrewScreen_stopIcon;

// Flushing state elements
extern lv_obj_t *ui_NewBrewScreen_flushingLabel;

// Complete state elements
extern lv_obj_t *ui_NewBrewScreen_completeBtn;

// Nav dots container
extern lv_obj_t *ui_NewBrewScreen_dotsContainer;

// Mode label
extern lv_obj_t *ui_NewBrewScreen_modeLabel;

// Navigation dots
extern lv_obj_t *ui_NewBrewScreen_dot1;
extern lv_obj_t *ui_NewBrewScreen_dot2;
extern lv_obj_t *ui_NewBrewScreen_dot3;

void ui_NewBrewScreen_screen_init(void);

// State management
void ui_NewBrewScreen_set_idle(void);
void ui_NewBrewScreen_set_flushing(void);
void ui_NewBrewScreen_set_brewing(void);
void ui_NewBrewScreen_set_complete(void);

#ifdef __cplusplus
}
#endif

#endif
