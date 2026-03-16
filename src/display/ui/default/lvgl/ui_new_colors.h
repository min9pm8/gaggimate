#ifndef UI_NEW_COLORS_H
#define UI_NEW_COLORS_H

#include "lvgl.h"

// === Opacity helper (LVGL 8.4 only has enum constants like LV_OPA_50) ===
#define UI_OPA(pct) ((lv_opa_t)((pct) * 255 / 100))

// === Standby Palette (Monochrome Luxury) ===
#define UI_COLOR_STANDBY_BG         lv_color_hex(0x000000)
#define UI_COLOR_STANDBY_ICON_PRI   lv_color_hex(0xFFFFFF)
#define UI_COLOR_STANDBY_ICON_SEC   lv_color_hex(0xB0B0B0)
#define UI_STANDBY_ICON_SEC_OPA     UI_OPA(50)

// === Active Palette (Material Dark) ===
#define UI_COLOR_BG                 lv_color_hex(0x000000)
#define UI_COLOR_SURFACE            lv_color_hex(0x1A1A1A)
#define UI_COLOR_TEXT_PRI           lv_color_hex(0xE0E0E0)
#define UI_COLOR_TEXT_SEC           lv_color_hex(0x666666)
#define UI_COLOR_TEXT_TER           lv_color_hex(0x555555)
#define UI_COLOR_MUTED             lv_color_hex(0x333333)
#define UI_COLOR_BTN_BORDER        lv_color_hex(0x666666)

// === Accent Colors ===
#define UI_COLOR_GREEN             lv_color_hex(0x00E676)
#define UI_COLOR_BLUE              lv_color_hex(0x448AFF)
#define UI_COLOR_RED               lv_color_hex(0xFF3B30)

// === Standby Icon States ===
#define UI_COLOR_ICON_DISCONNECTED lv_color_hex(0xFF3B30)

// === Sizes (in pixels, for 466x466 display) ===
#define UI_STANDBY_ICON_SIZE       64
#define UI_STANDBY_ICON_GAP        40

#define UI_FLUSH_BTN_SIZE          48
#define UI_FLUSH_BTN_ACTIVE_SIZE   56
#define UI_BREW_BTN_SIZE           52
#define UI_STOP_BTN_SIZE           48
#define UI_PUMP_BTN_SIZE           50

#define UI_RING_OUTER_RADIUS       125
#define UI_RING_OUTER_WIDTH        8
#define UI_RING_INNER_RADIUS       108
#define UI_RING_INNER_WIDTH        6
#define UI_RING_SINGLE_RADIUS      118
#define UI_RING_SINGLE_WIDTH       8

#define UI_NAV_DOT_SIZE            5
#define UI_NAV_DOT_GAP             5
#define UI_NAV_DOT_BOTTOM          16

// === Arc Range (270 degrees, gap at bottom) ===
#define UI_ARC_START_ANGLE         135
#define UI_ARC_END_ANGLE           45

// === Gauge Ranges ===
#define UI_PRESSURE_MIN            0
#define UI_PRESSURE_MAX            12
#define UI_TEMP_BREW_MIN           0
#define UI_TEMP_BREW_MAX           110
#define UI_TEMP_WATER_MIN          0
#define UI_TEMP_WATER_MAX          100
#define UI_TEMP_STEAM_MIN          0
#define UI_TEMP_STEAM_MAX          160

// === Animation Durations ===
#define UI_ANIM_FADE_MS            300
#define UI_ANIM_SLIDE_MS           250
#define UI_ANIM_BOUNCE_MS          150
#define UI_ANIM_RING_MS            200
#define UI_BREW_COMPLETE_DISMISS_MS 5000

#endif // UI_NEW_COLORS_H
