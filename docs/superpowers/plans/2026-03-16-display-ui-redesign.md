# Display UI Redesign Implementation Plan

> **For agentic workers:** REQUIRED: Use superpowers:subagent-driven-development (if subagents available) or superpowers:executing-plans to implement this plan. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace the SquareLine Studio-generated UI with a hand-coded LVGL UI optimized for the 1.43" round AMOLED — 4 screens (Standby, Brew, Hot Water, Steam) with swipe navigation, concentric ring gauges, and a monochrome-to-vibrant color transition.

**Architecture:** New UI screens are hand-coded LVGL C files that replace the existing SquareLine-generated screens. DefaultUI.cpp is refactored to manage the new 4-screen flow with swipe-based navigation instead of the current menu-driven approach. The existing event-driven architecture (PluginManager) and Controller integration remain unchanged — only the screen layer and DefaultUI screen management change.

**Tech Stack:** LVGL 8.4.0, ESP32-S3 (Arduino framework), C/C++, PlatformIO

**Spec:** `docs/superpowers/specs/2026-03-16-display-ui-redesign-design.md`

---

## File Structure

### New files to create:
- `src/display/ui/default/lvgl/screens/ui_NewStandbyScreen.c` — Standby screen (WiFi/BT icons only)
- `src/display/ui/default/lvgl/screens/ui_NewStandbyScreen.h` — Standby screen header
- `src/display/ui/default/lvgl/screens/ui_NewBrewScreen.c` — Brew screen (concentric rings, flush/brew buttons)
- `src/display/ui/default/lvgl/screens/ui_NewBrewScreen.h` — Brew screen header
- `src/display/ui/default/lvgl/screens/ui_NewWaterScreen.c` — Hot Water screen (single ring, pump button)
- `src/display/ui/default/lvgl/screens/ui_NewWaterScreen.h` — Hot Water screen header
- `src/display/ui/default/lvgl/screens/ui_NewSteamScreen.c` — Steam screen (single ring, no button)
- `src/display/ui/default/lvgl/screens/ui_NewSteamScreen.h` — Steam screen header
- `src/display/ui/default/lvgl/ui_new_colors.h` — Color constants and theme tokens
- `src/display/ui/default/lvgl/ui_new_events.cpp` — New event handlers (swipe, flush, brew, pump)
- `src/display/ui/default/lvgl/ui_new_events.h` — New event handler declarations

### Files to modify:
- `src/display/lv_conf.h:364-384` — Enable Montserrat 10, 12, 16, 48
- `src/display/core/constants.h:14` — Change DEFAULT_STANDBY_TIMEOUT_MS to 60000
- `src/display/ui/default/DefaultUI.h` — New screen pointers, remove unused state, add swipe nav methods
- `src/display/ui/default/DefaultUI.cpp` — Refactor screen management, event subscriptions, reactive updates
- `src/display/ui/default/lvgl/ui.h` — Include new screen headers instead of old ones

---

## Chunk 1: Foundation — Fonts, Colors, Constants

### Task 1: Enable required fonts in lv_conf.h

**Files:**
- Modify: `src/display/lv_conf.h:364-384`

- [ ] **Step 1: Enable Montserrat 10, 12, 16, 48**

Change these lines in `src/display/lv_conf.h`:

```c
#define LV_FONT_MONTSERRAT_10 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_48 1
```

Keep existing enabled fonts (14, 18, 20, 24, 34) — they're used by the existing UI and we need backward compatibility during transition.

- [ ] **Step 2: Verify build compiles**

Run: `pio run -e amoled`
Expected: Compiles without errors. Watch for flash size warnings — Montserrat 48 adds ~60-80KB.

- [ ] **Step 3: Commit**

```bash
git add src/display/lv_conf.h
git commit -m "feat(ui): enable Montserrat 10, 12, 16, 48 fonts for redesign"
```

### Task 2: Create color constants header

**Files:**
- Create: `src/display/ui/default/lvgl/ui_new_colors.h`

- [ ] **Step 1: Create the color constants file**

```c
#ifndef UI_NEW_COLORS_H
#define UI_NEW_COLORS_H

#include "lvgl.h"

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

// === Opacity helper (LVGL 8.4 only has enum constants) ===
#define UI_OPA(pct) ((lv_opa_t)((pct) * 255 / 100))

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
```

- [ ] **Step 2: Verify build compiles**

Run: `pio run -e amoled`
Expected: Compiles (header not yet included anywhere).

- [ ] **Step 3: Commit**

```bash
git add src/display/ui/default/lvgl/ui_new_colors.h
git commit -m "feat(ui): add color constants and sizing for redesigned UI"
```

### Task 3: Update standby timeout default

**Files:**
- Modify: `src/display/core/constants.h:14`

- [ ] **Step 1: Change DEFAULT_STANDBY_TIMEOUT_MS**

In `src/display/core/constants.h`, change line 14:

```c
#define DEFAULT_STANDBY_TIMEOUT_MS 60000
```

- [ ] **Step 2: Commit**

```bash
git add src/display/core/constants.h
git commit -m "feat(ui): change standby timeout default to 60 seconds"
```

### Task 3b: Create event handler stubs (needed before screens)

**Files:**
- Create: `src/display/ui/default/lvgl/ui_new_events.h`
- Create: `src/display/ui/default/lvgl/ui_new_events.cpp`

These must exist before screen files so that screen `_init()` functions can reference event callbacks.

- [ ] **Step 1: Create event handler header (declarations)**

```c
// ui_new_events.h
#ifndef UI_NEW_EVENTS_H
#define UI_NEW_EVENTS_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void ui_event_NewStandbyScreen(lv_event_t *e);
void ui_event_NewBrewScreen_gesture(lv_event_t *e);
void ui_event_NewBrewScreen_flush(lv_event_t *e);
void ui_event_NewBrewScreen_brew(lv_event_t *e);
void ui_event_NewBrewScreen_stop(lv_event_t *e);
void ui_event_NewBrewScreen_complete_dismiss(lv_event_t *e);
void ui_event_NewWaterScreen_gesture(lv_event_t *e);
void ui_event_NewWaterScreen_pump(lv_event_t *e);
void ui_event_NewSteamScreen_gesture(lv_event_t *e);

#ifdef __cplusplus
}
#endif

#endif
```

- [ ] **Step 2: Create stub implementations**

```cpp
// ui_new_events.cpp — stubs, will be filled in Chunk 5
#include "ui_new_events.h"

void ui_event_NewStandbyScreen(lv_event_t *e) {}
void ui_event_NewBrewScreen_gesture(lv_event_t *e) {}
void ui_event_NewBrewScreen_flush(lv_event_t *e) {}
void ui_event_NewBrewScreen_brew(lv_event_t *e) {}
void ui_event_NewBrewScreen_stop(lv_event_t *e) {}
void ui_event_NewBrewScreen_complete_dismiss(lv_event_t *e) {}
void ui_event_NewWaterScreen_gesture(lv_event_t *e) {}
void ui_event_NewWaterScreen_pump(lv_event_t *e) {}
void ui_event_NewSteamScreen_gesture(lv_event_t *e) {}
```

- [ ] **Step 3: Commit**

```bash
git add src/display/ui/default/lvgl/ui_new_events.*
git commit -m "feat(ui): add event handler stubs for new screens"
```

---

## Chunk 2: Standby Screen

### Task 4: Create the Standby screen

**Files:**
- Create: `src/display/ui/default/lvgl/screens/ui_NewStandbyScreen.h`
- Create: `src/display/ui/default/lvgl/screens/ui_NewStandbyScreen.c`

- [ ] **Step 1: Create the header file**

```c
// ui_NewStandbyScreen.h
#ifndef UI_NEW_STANDBY_SCREEN_H
#define UI_NEW_STANDBY_SCREEN_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t *ui_NewStandbyScreen;
extern lv_obj_t *ui_NewStandbyScreen_wifiIcon;
extern lv_obj_t *ui_NewStandbyScreen_btIcon;

void ui_NewStandbyScreen_screen_init(void);

#ifdef __cplusplus
}
#endif

#endif
```

- [ ] **Step 2: Create the screen implementation**

```c
// ui_NewStandbyScreen.c
#include "ui_NewStandbyScreen.h"
#include "../ui_new_colors.h"

// WiFi icon paths (Material Design style, simplified for LVGL canvas)
// We'll use LVGL symbol fonts or custom draw for icons

lv_obj_t *ui_NewStandbyScreen = NULL;
lv_obj_t *ui_NewStandbyScreen_wifiIcon = NULL;
lv_obj_t *ui_NewStandbyScreen_btIcon = NULL;

void ui_NewStandbyScreen_screen_init(void) {
    ui_NewStandbyScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_NewStandbyScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_NewStandbyScreen, UI_COLOR_STANDBY_BG, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewStandbyScreen, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(ui_NewStandbyScreen, LV_RADIUS_CIRCLE, LV_PART_MAIN);

    // Container for centering icons
    lv_obj_t *icon_container = lv_obj_create(ui_NewStandbyScreen);
    lv_obj_remove_style_all(icon_container);
    lv_obj_set_size(icon_container, UI_STANDBY_ICON_SIZE * 2 + UI_STANDBY_ICON_GAP, UI_STANDBY_ICON_SIZE);
    lv_obj_center(icon_container);
    lv_obj_set_flex_flow(icon_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(icon_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(icon_container, UI_STANDBY_ICON_GAP, LV_PART_MAIN);

    // WiFi icon — use existing asset (scaled up) or LVGL symbol
    // Using existing wifi image asset, scaled via img zoom
    ui_NewStandbyScreen_wifiIcon = lv_img_create(icon_container);
    lv_img_set_src(ui_NewStandbyScreen_wifiIcon, &ui_img_364513079); // wifi-20x20.png
    lv_img_set_zoom(ui_NewStandbyScreen_wifiIcon, 256 * 64 / 20);   // Scale 20px -> 64px
    lv_obj_set_style_img_recolor(ui_NewStandbyScreen_wifiIcon, UI_COLOR_STANDBY_ICON_PRI, LV_PART_MAIN);
    lv_obj_set_style_img_recolor_opa(ui_NewStandbyScreen_wifiIcon, LV_OPA_COVER, LV_PART_MAIN);

    // Bluetooth icon
    ui_NewStandbyScreen_btIcon = lv_img_create(icon_container);
    lv_img_set_src(ui_NewStandbyScreen_btIcon, &ui_img_1091371356); // bluetooth-alt-20x20.png
    lv_img_set_zoom(ui_NewStandbyScreen_btIcon, 256 * 64 / 20);     // Scale 20px -> 64px
    lv_obj_set_style_img_recolor(ui_NewStandbyScreen_btIcon, UI_COLOR_STANDBY_ICON_SEC, LV_PART_MAIN);
    lv_obj_set_style_img_recolor_opa(ui_NewStandbyScreen_btIcon, UI_STANDBY_ICON_SEC_OPA, LV_PART_MAIN);

    // Tap anywhere gesture — handled by event callback in DefaultUI
    lv_obj_add_event_cb(ui_NewStandbyScreen, ui_event_NewStandbyScreen, LV_EVENT_CLICKED, NULL);
}
```

**Note:** The event callbacks referenced here (`ui_event_NewStandbyScreen`, etc.) are declared in `ui_new_events.h` which must be created FIRST (see Task 3b below). All screen `.c` files should `#include "../ui_new_events.h"` at the top.

The image scaling approach (zoom from 20px assets to 64px) may produce blurry results — if so, the implementation should create dedicated 64px icon assets. This is an implementation detail to verify on hardware.

- [ ] **Step 3: Verify build compiles**

Run: `pio run -e amoled`
Expected: Compiles (event handler declarations exist in ui_new_events.h from Task 3b, implementations are stubs).

- [ ] **Step 4: Commit**

```bash
git add src/display/ui/default/lvgl/screens/ui_NewStandbyScreen.*
git commit -m "feat(ui): add new standby screen — monochrome WiFi/BT icons"
```

---

## Chunk 3: Brew Screen (Idle, Flushing, Brewing, Complete States)

### Task 5: Create the Brew screen

**Files:**
- Create: `src/display/ui/default/lvgl/screens/ui_NewBrewScreen.h`
- Create: `src/display/ui/default/lvgl/screens/ui_NewBrewScreen.c`

- [ ] **Step 1: Create the header file**

```c
// ui_NewBrewScreen.h
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

// Nav dots container (needs to be hidden during active states)
extern lv_obj_t *ui_NewBrewScreen_dotsContainer;

// Mode label
extern lv_obj_t *ui_NewBrewScreen_modeLabel;

// Navigation dots
extern lv_obj_t *ui_NewBrewScreen_dot1;
extern lv_obj_t *ui_NewBrewScreen_dot2;
extern lv_obj_t *ui_NewBrewScreen_dot3;

void ui_NewBrewScreen_screen_init(void);

// State management (called from DefaultUI)
void ui_NewBrewScreen_set_idle(void);
void ui_NewBrewScreen_set_flushing(void);
void ui_NewBrewScreen_set_brewing(void);
void ui_NewBrewScreen_set_complete(void);

#ifdef __cplusplus
}
#endif

#endif
```

- [ ] **Step 2: Create the screen implementation**

This is the most complex screen. Key implementation details:

```c
// ui_NewBrewScreen.c
#include "ui_NewBrewScreen.h"
#include "../ui_new_colors.h"

// All object pointers initialized to NULL
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
    lv_obj_set_style_radius(ui_NewBrewScreen, LV_RADIUS_CIRCLE, LV_PART_MAIN);

    int cx = 233; // Center of 466px display
    int cy = 233;

    // --- Mode label at top ---
    ui_NewBrewScreen_modeLabel = lv_label_create(ui_NewBrewScreen);
    lv_label_set_text(ui_NewBrewScreen_modeLabel, "BREW");
    lv_obj_set_style_text_font(ui_NewBrewScreen_modeLabel, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewBrewScreen_modeLabel, UI_COLOR_TEXT_SEC, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_NewBrewScreen_modeLabel, 2, LV_PART_MAIN);
    lv_obj_align(ui_NewBrewScreen_modeLabel, LV_ALIGN_TOP_MID, 0, 40);

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
    // Track (background)
    lv_obj_set_style_arc_color(ui_NewBrewScreen_pressureArc, UI_COLOR_SURFACE, LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui_NewBrewScreen_pressureArc, UI_RING_OUTER_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(ui_NewBrewScreen_pressureArc, true, LV_PART_MAIN);
    // Indicator (fill)
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
    lv_obj_set_size(center_stack, 120, 260);
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
    // Flush icon — water droplet (use existing asset or LV_SYMBOL)
    ui_NewBrewScreen_flushIcon = lv_label_create(ui_NewBrewScreen_flushBtn);
    lv_label_set_text(ui_NewBrewScreen_flushIcon, LV_SYMBOL_TINT);
    lv_obj_set_style_text_font(ui_NewBrewScreen_flushIcon, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewBrewScreen_flushIcon, UI_COLOR_TEXT_SEC, LV_PART_MAIN);
    lv_obj_center(ui_NewBrewScreen_flushIcon);
    lv_obj_add_event_cb(ui_NewBrewScreen_flushBtn, ui_event_NewBrewScreen_flush, LV_EVENT_CLICKED, NULL);

    // --- Flushing label (right after flush btn in flex order, hidden in idle) ---
    ui_NewBrewScreen_flushingLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_NewBrewScreen_flushingLabel, "FLUSHING");
    lv_obj_set_style_text_font(ui_NewBrewScreen_flushingLabel, &lv_font_montserrat_10, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewBrewScreen_flushingLabel, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_NewBrewScreen_flushingLabel, 2, LV_PART_MAIN);
    lv_obj_add_flag(ui_NewBrewScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);

    // --- Timer label (hidden in idle, replaces flush position during brewing) ---
    ui_NewBrewScreen_timerLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_NewBrewScreen_timerLabel, "0:00");
    lv_obj_set_style_text_font(ui_NewBrewScreen_timerLabel, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewBrewScreen_timerLabel, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_text_letter_space(ui_NewBrewScreen_timerLabel, 1, LV_PART_MAIN);
    lv_obj_add_flag(ui_NewBrewScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);

    // --- Temperature label ---
    ui_NewBrewScreen_tempLabel = lv_label_create(center_stack);
    lv_label_set_text(ui_NewBrewScreen_tempLabel, "0°");
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
    // Play triangle icon
    ui_NewBrewScreen_brewIcon = lv_label_create(ui_NewBrewScreen_brewBtn);
    lv_label_set_text(ui_NewBrewScreen_brewIcon, LV_SYMBOL_PLAY);
    lv_obj_set_style_text_font(ui_NewBrewScreen_brewIcon, &lv_font_montserrat_16, LV_PART_MAIN);
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
    lv_obj_set_style_text_font(ui_NewBrewScreen_stopIcon, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NewBrewScreen_stopIcon, UI_COLOR_RED, LV_PART_MAIN);
    lv_obj_center(ui_NewBrewScreen_stopIcon);
    lv_obj_add_flag(ui_NewBrewScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(ui_NewBrewScreen_stopBtn, ui_event_NewBrewScreen_stop, LV_EVENT_CLICKED, NULL);

    // --- Complete dismiss button (hidden, shown after brew completes) ---
    ui_NewBrewScreen_completeBtn = lv_btn_create(center_stack);
    lv_obj_set_size(ui_NewBrewScreen_completeBtn, UI_STOP_BTN_SIZE, UI_STOP_BTN_SIZE);
    lv_obj_set_style_radius(ui_NewBrewScreen_completeBtn, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewBrewScreen_completeBtn, UI_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewBrewScreen_completeBtn, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_NewBrewScreen_completeBtn, 0, LV_PART_MAIN);
    lv_obj_t *checkLabel = lv_label_create(ui_NewBrewScreen_completeBtn);
    lv_label_set_text(checkLabel, LV_SYMBOL_OK);
    lv_obj_set_style_text_font(checkLabel, &lv_font_montserrat_16, LV_PART_MAIN);
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

    // --- Gesture handler for swipe navigation ---
    lv_obj_add_event_cb(ui_NewBrewScreen, ui_event_NewBrewScreen_gesture, LV_EVENT_GESTURE, NULL);

    // Start in idle state
    ui_NewBrewScreen_set_idle();
}

// === State Management Functions ===

void ui_NewBrewScreen_set_idle(void) {
    // Show: flush btn, temp, pressure, brew btn, dots
    lv_obj_clear_flag(ui_NewBrewScreen_flushBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_tempLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_pressureLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_brewBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_dotsContainer, LV_OBJ_FLAG_HIDDEN);

    // Hide: timer, phase, stop, flushing label, complete
    lv_obj_add_flag(ui_NewBrewScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_completeBtn, LV_OBJ_FLAG_HIDDEN);

    // Restore ring opacity
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureArc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureArc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, LV_OPA_COVER, LV_PART_INDICATOR);

    // Restore flush button idle style
    lv_obj_set_size(ui_NewBrewScreen_flushBtn, UI_FLUSH_BTN_SIZE, UI_FLUSH_BTN_SIZE);
    lv_obj_set_style_border_color(ui_NewBrewScreen_flushBtn, UI_COLOR_BTN_BORDER, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewBrewScreen_flushBtn, UI_OPA(3), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_NewBrewScreen_flushBtn, 0, LV_PART_MAIN);
}

void ui_NewBrewScreen_set_flushing(void) {
    // Show: flush btn (active style), flushing label, temp (dimmed), pressure (dimmed)
    lv_obj_clear_flag(ui_NewBrewScreen_flushBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_tempLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_pressureLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);

    // Hide: brew btn, timer, phase, stop, dots
    lv_obj_add_flag(ui_NewBrewScreen_brewBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_dotsContainer, LV_OBJ_FLAG_HIDDEN);

    // Flush button active style
    lv_obj_set_size(ui_NewBrewScreen_flushBtn, UI_FLUSH_BTN_ACTIVE_SIZE, UI_FLUSH_BTN_ACTIVE_SIZE);
    lv_obj_set_style_border_color(ui_NewBrewScreen_flushBtn, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_NewBrewScreen_flushBtn, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NewBrewScreen_flushBtn, UI_OPA(15), LV_PART_MAIN);
    lv_obj_set_style_shadow_color(ui_NewBrewScreen_flushBtn, UI_COLOR_BLUE, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(ui_NewBrewScreen_flushBtn, 24, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(ui_NewBrewScreen_flushBtn, UI_OPA(40), LV_PART_MAIN);
    // Change icon to stop square
    lv_label_set_text(ui_NewBrewScreen_flushIcon, LV_SYMBOL_STOP);
    lv_obj_set_style_text_color(ui_NewBrewScreen_flushIcon, UI_COLOR_BLUE, LV_PART_MAIN);

    // Dim rings
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureArc, UI_OPA(30), LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureArc, UI_OPA(30), LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, UI_OPA(30), LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, UI_OPA(30), LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, UI_OPA(30), LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureArc, UI_OPA(30), LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, UI_OPA(30), LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, UI_OPA(30), LV_PART_INDICATOR);

    // Dim text
    lv_obj_set_style_opa(ui_NewBrewScreen_tempLabel, UI_OPA(60), LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureLabel, UI_OPA(60), LV_PART_MAIN);
}

void ui_NewBrewScreen_set_brewing(void) {
    // Show: timer, temp, pressure, phase, stop btn
    lv_obj_clear_flag(ui_NewBrewScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_tempLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_pressureLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);

    // Hide: flush btn, brew btn, flushing label, dots
    lv_obj_add_flag(ui_NewBrewScreen_flushBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_brewBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_dotsContainer, LV_OBJ_FLAG_HIDDEN);

    // Restore ring and text opacity
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureArc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureArc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempArc, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_opa(ui_NewBrewScreen_tempLabel, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(ui_NewBrewScreen_pressureLabel, LV_OPA_COVER, LV_PART_MAIN);
}

void ui_NewBrewScreen_set_complete(void) {
    // Show: timer (frozen), temp, pressure, phase "COMPLETE"
    lv_obj_clear_flag(ui_NewBrewScreen_timerLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_tempLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_pressureLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_NewBrewScreen_phaseLabel, LV_OBJ_FLAG_HIDDEN);

    // Show: complete dismiss button
    lv_obj_clear_flag(ui_NewBrewScreen_completeBtn, LV_OBJ_FLAG_HIDDEN);

    // Hide: flush, brew, stop, flushing label
    lv_obj_add_flag(ui_NewBrewScreen_flushBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_brewBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_stopBtn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_flushingLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_NewBrewScreen_dotsContainer, LV_OBJ_FLAG_HIDDEN);

    // Timer turns white
    lv_obj_set_style_text_color(ui_NewBrewScreen_timerLabel, UI_COLOR_STANDBY_ICON_PRI, LV_PART_MAIN);
    lv_label_set_text(ui_NewBrewScreen_phaseLabel, "COMPLETE");
}
```

- [ ] **Step 3: Verify build compiles**

Run: `pio run -e amoled`
Expected: Will have unresolved event callback references — that's expected until Task 7.

- [ ] **Step 4: Commit**

```bash
git add src/display/ui/default/lvgl/screens/ui_NewBrewScreen.*
git commit -m "feat(ui): add new brew screen with idle/flushing/brewing/complete states"
```

---

## Chunk 4: Water and Steam Screens

### Task 6: Create Hot Water and Steam screens

**Files:**
- Create: `src/display/ui/default/lvgl/screens/ui_NewWaterScreen.h`
- Create: `src/display/ui/default/lvgl/screens/ui_NewWaterScreen.c`
- Create: `src/display/ui/default/lvgl/screens/ui_NewSteamScreen.h`
- Create: `src/display/ui/default/lvgl/screens/ui_NewSteamScreen.c`

- [ ] **Step 1: Create Hot Water screen header**

```c
// ui_NewWaterScreen.h
#ifndef UI_NEW_WATER_SCREEN_H
#define UI_NEW_WATER_SCREEN_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t *ui_NewWaterScreen;
extern lv_obj_t *ui_NewWaterScreen_tempArc;
extern lv_obj_t *ui_NewWaterScreen_targetLabel;
extern lv_obj_t *ui_NewWaterScreen_tempLabel;
extern lv_obj_t *ui_NewWaterScreen_statusLabel;
extern lv_obj_t *ui_NewWaterScreen_pumpBtn;
extern lv_obj_t *ui_NewWaterScreen_modeLabel;
extern lv_obj_t *ui_NewWaterScreen_dot1;
extern lv_obj_t *ui_NewWaterScreen_dot2;
extern lv_obj_t *ui_NewWaterScreen_dot3;

void ui_NewWaterScreen_screen_init(void);

#ifdef __cplusplus
}
#endif

#endif
```

- [ ] **Step 2: Create Hot Water screen implementation**

Follow the same pattern as Brew screen but with:
- Single blue ring (`UI_COLOR_BLUE`), radius `UI_RING_SINGLE_RADIUS`, width `UI_RING_SINGLE_WIDTH`
- Range: `UI_TEMP_WATER_MIN` to `UI_TEMP_WATER_MAX`
- Target label: Montserrat 12, `UI_COLOR_TEXT_TER`, shows `Settings::targetWaterTemp`
- Temp label: Montserrat 48, `UI_COLOR_TEXT_PRI`
- Status label: Montserrat 12, `UI_COLOR_BLUE` ("heating..." / "ready")
- Pump button: 50px circle, solid `UI_COLOR_BLUE`, droplet icon, blue shadow
- Mode label: "HOT WATER"
- Nav dots: middle dot active in `UI_COLOR_BLUE`
- Gesture handler for swipe left (→ Brew) and swipe right (→ Steam) and swipe down (→ Standby)

- [ ] **Step 3: Create Steam screen header**

```c
// ui_NewSteamScreen.h
#ifndef UI_NEW_STEAM_SCREEN_H
#define UI_NEW_STEAM_SCREEN_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t *ui_NewSteamScreen;
extern lv_obj_t *ui_NewSteamScreen_tempArc;
extern lv_obj_t *ui_NewSteamScreen_targetLabel;
extern lv_obj_t *ui_NewSteamScreen_tempLabel;
extern lv_obj_t *ui_NewSteamScreen_statusLabel;
extern lv_obj_t *ui_NewSteamScreen_modeLabel;
extern lv_obj_t *ui_NewSteamScreen_dot1;
extern lv_obj_t *ui_NewSteamScreen_dot2;
extern lv_obj_t *ui_NewSteamScreen_dot3;

void ui_NewSteamScreen_screen_init(void);

#ifdef __cplusplus
}
#endif

#endif
```

- [ ] **Step 4: Create Steam screen implementation**

Follow same pattern but with:
- Single ring, color changes: `UI_COLOR_RED` while heating → `UI_COLOR_GREEN` when `currentTemp >= targetTemp`
- Range: `UI_TEMP_STEAM_MIN` to `UI_TEMP_STEAM_MAX`
- **No action button** — steam is physical
- Mode label: "STEAM"
- Nav dots: third dot active in `UI_COLOR_RED` (or green when ready)
- Gesture handler: swipe left (→ Water), swipe down (→ Standby), swipe right (no-op with bounce)

- [ ] **Step 5: Commit**

```bash
git add src/display/ui/default/lvgl/screens/ui_NewWaterScreen.* src/display/ui/default/lvgl/screens/ui_NewSteamScreen.*
git commit -m "feat(ui): add hot water and steam screens with ring gauges"
```

---

## Chunk 5: Event Handler Implementations

### Task 7: Implement event handlers (replace stubs)

**Files:**
- Modify: `src/display/ui/default/lvgl/ui_new_events.cpp` (replace stubs from Task 3b)

- [ ] **Step 1: Replace stub implementations with real handlers**

```cpp
// ui_new_events.cpp
#include "ui_new_events.h"
#include "../../../main.h"  // Access to controller global
#include "screens/ui_NewStandbyScreen.h"
#include "screens/ui_NewBrewScreen.h"
#include "screens/ui_NewWaterScreen.h"
#include "screens/ui_NewSteamScreen.h"

// --- Standby ---
void ui_event_NewStandbyScreen(lv_event_t *e) {
    // Tap anywhere → Brew
    controller.getUI()->changeScreen(&ui_NewBrewScreen, &ui_NewBrewScreen_screen_init);
    controller.setMode(MODE_BREW);
}

// --- Brew gestures ---
void ui_event_NewBrewScreen_gesture(lv_event_t *e) {
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    lv_indev_wait_release(lv_indev_get_act());

    if (dir == LV_DIR_RIGHT) {
        // Brew → Water
        controller.deactivate();
        controller.getUI()->changeScreen(&ui_NewWaterScreen, &ui_NewWaterScreen_screen_init);
        controller.setMode(MODE_WATER);
    } else if (dir == LV_DIR_BOTTOM) {
        // Any → Standby (stops active operations including flush)
        if (controller.isFlushing()) {
            controller.onFlush(); // Stop flush
        }
        controller.deactivate();
        controller.clear();
        controller.activateStandby();
    }
    // LV_DIR_LEFT from Brew = no-op (edge)
}

void ui_event_NewBrewScreen_flush(lv_event_t *e) {
    controller.onFlush();  // Toggles flush on/off
}

void ui_event_NewBrewScreen_brew(lv_event_t *e) {
    controller.activate();
}

void ui_event_NewBrewScreen_stop(lv_event_t *e) {
    controller.deactivate();
    controller.clear();
}

void ui_event_NewBrewScreen_complete_dismiss(lv_event_t *e) {
    // Dismiss brew complete, return to idle
    ui_NewBrewScreen_set_idle();
}

// --- Water gestures ---
void ui_event_NewWaterScreen_gesture(lv_event_t *e) {
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    lv_indev_wait_release(lv_indev_get_act());

    if (dir == LV_DIR_LEFT) {
        // Water → Brew
        controller.deactivate();
        controller.getUI()->changeScreen(&ui_NewBrewScreen, &ui_NewBrewScreen_screen_init);
        controller.setMode(MODE_BREW);
    } else if (dir == LV_DIR_RIGHT) {
        // Water → Steam
        controller.deactivate();
        controller.getUI()->changeScreen(&ui_NewSteamScreen, &ui_NewSteamScreen_screen_init);
        controller.setMode(MODE_STEAM);
    } else if (dir == LV_DIR_BOTTOM) {
        controller.deactivate();
        controller.activateStandby();
    }
}

void ui_event_NewWaterScreen_pump(lv_event_t *e) {
    controller.isActive() ? controller.deactivate() : controller.activate();
}

// --- Steam gestures ---
void ui_event_NewSteamScreen_gesture(lv_event_t *e) {
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    lv_indev_wait_release(lv_indev_get_act());

    if (dir == LV_DIR_LEFT) {
        // Steam → Water
        controller.getUI()->changeScreen(&ui_NewWaterScreen, &ui_NewWaterScreen_screen_init);
        controller.setMode(MODE_WATER);
    } else if (dir == LV_DIR_BOTTOM) {
        controller.activateStandby();
    }
    // LV_DIR_RIGHT from Steam = no-op (edge)
}
```

- [ ] **Step 3: Commit**

```bash
git add src/display/ui/default/lvgl/ui_new_events.*
git commit -m "feat(ui): add swipe navigation and action event handlers"
```

---

## Chunk 6: Wire Up DefaultUI

### Task 8: Refactor DefaultUI to use new screens

This is the integration task. Modify DefaultUI to use the new 4-screen flow.

**Files:**
- Modify: `src/display/ui/default/DefaultUI.h`
- Modify: `src/display/ui/default/DefaultUI.cpp`
- Modify: `src/display/ui/default/lvgl/ui.h`

- [ ] **Step 1: Update ui.h to include new screen headers**

Add new includes alongside existing ones (don't remove old ones yet — they may be needed for compilation):

```c
// Add after existing screen includes in ui.h
#include "screens/ui_NewStandbyScreen.h"
#include "screens/ui_NewBrewScreen.h"
#include "screens/ui_NewWaterScreen.h"
#include "screens/ui_NewSteamScreen.h"
#include "ui_new_events.h"
#include "ui_new_colors.h"
```

- [ ] **Step 2: Update DefaultUI.h**

Add new state tracking and methods. Key changes:

```cpp
// Add to DefaultUI private section:
bool isFlushing = false;
bool isBrewing = false;
bool isBrewComplete = false;
unsigned long brewCompleteTime = 0;

// Add method declarations:
void updateNewBrewScreen();
void updateNewWaterScreen();
void updateNewSteamScreen();
void updateNewStandbyScreen();
```

- [ ] **Step 3: Update DefaultUI::init() event subscriptions**

Modify the mode change handler (around line 123-143 of DefaultUI.cpp) to route to new screens:

```cpp
// Replace the mode change handler:
pluginManager->on("controller:mode:change", [this](const PluginEvent &event) {
    int newMode = std::stoi(event.data);
    mode = newMode;
    switch (newMode) {
        case MODE_STANDBY:
            changeScreen(&ui_NewStandbyScreen, &ui_NewStandbyScreen_screen_init);
            break;
        case MODE_BREW:
            changeScreen(&ui_NewBrewScreen, &ui_NewBrewScreen_screen_init);
            break;
        case MODE_WATER:
            changeScreen(&ui_NewWaterScreen, &ui_NewWaterScreen_screen_init);
            break;
        case MODE_STEAM:
            changeScreen(&ui_NewSteamScreen, &ui_NewSteamScreen_screen_init);
            break;
        default:
            break;
    }
});
```

Update brew start/clear handlers:

```cpp
pluginManager->on("controller:brew:start", [this](const PluginEvent &event) {
    isBrewing = true;
    isBrewComplete = false;
    if (currentScreen == ui_NewBrewScreen) {
        ui_NewBrewScreen_set_brewing();
    }
    rerender = true;
});

pluginManager->on("controller:brew:clear", [this](const PluginEvent &event) {
    if (isBrewing && !isBrewComplete) {
        // Brew was stopped manually
        isBrewing = false;
        if (currentScreen == ui_NewBrewScreen) {
            ui_NewBrewScreen_set_idle();
        }
    }
    rerender = true;
});
```

- [ ] **Step 4: Update DefaultUI::setupReactive()**

Replace existing reactive effects with new screen update logic. The key effects to add:

```cpp
// Temperature update for new brew screen
effect_mgr.use_effect(
    [=] { return currentScreen == ui_NewBrewScreen; },
    [=]() {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d°", currentTemp);
        lv_label_set_text(ui_NewBrewScreen_tempLabel, buf);
        lv_arc_set_value(ui_NewBrewScreen_tempArc, currentTemp);
    },
    &currentTemp
);

// Pressure update for new brew screen
effect_mgr.use_effect(
    [=] { return currentScreen == ui_NewBrewScreen && pressureAvailable; },
    [=]() {
        char buf[16];
        snprintf(buf, sizeof(buf), "%.1f bar", pressure);
        lv_label_set_text(ui_NewBrewScreen_pressureLabel, buf);
        lv_arc_set_value(ui_NewBrewScreen_pressureArc, (int)(pressure * 10));
    },
    &pressure
);

// Temperature update for water screen
effect_mgr.use_effect(
    [=] { return currentScreen == ui_NewWaterScreen; },
    [=]() {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d°", currentTemp);
        lv_label_set_text(ui_NewWaterScreen_tempLabel, buf);
        lv_arc_set_value(ui_NewWaterScreen_tempArc, currentTemp);

        // Update status
        int target = controller->getTargetTemp();
        if (currentTemp >= target) {
            lv_label_set_text(ui_NewWaterScreen_statusLabel, "ready");
            lv_obj_set_style_text_color(ui_NewWaterScreen_statusLabel, UI_COLOR_GREEN, LV_PART_MAIN);
        } else {
            lv_label_set_text(ui_NewWaterScreen_statusLabel, "heating...");
            lv_obj_set_style_text_color(ui_NewWaterScreen_statusLabel, UI_COLOR_BLUE, LV_PART_MAIN);
        }
    },
    &currentTemp
);

// Temperature update for steam screen
effect_mgr.use_effect(
    [=] { return currentScreen == ui_NewSteamScreen; },
    [=]() {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d°", currentTemp);
        lv_label_set_text(ui_NewSteamScreen_tempLabel, buf);
        lv_arc_set_value(ui_NewSteamScreen_tempArc, currentTemp);

        int target = controller->getTargetTemp();
        bool ready = currentTemp >= target;
        // Change ring color based on ready state
        lv_color_t ringColor = ready ? UI_COLOR_GREEN : UI_COLOR_RED;
        lv_obj_set_style_arc_color(ui_NewSteamScreen_tempArc, ringColor, LV_PART_INDICATOR);

        if (ready) {
            lv_label_set_text(ui_NewSteamScreen_statusLabel, "ready");
            lv_obj_set_style_text_color(ui_NewSteamScreen_statusLabel, UI_COLOR_GREEN, LV_PART_MAIN);
        } else {
            lv_label_set_text(ui_NewSteamScreen_statusLabel, "heating...");
            lv_obj_set_style_text_color(ui_NewSteamScreen_statusLabel, UI_COLOR_RED, LV_PART_MAIN);
        }
    },
    &currentTemp
);
```

- [ ] **Step 5: Update standby icon states**

Add reactive effect for WiFi/BT icon coloring on standby:

```cpp
effect_mgr.use_effect(
    [=] { return currentScreen == ui_NewStandbyScreen; },
    [=]() {
        // WiFi icon state
        bool wifiConnected = WiFi.status() == WL_CONNECTED;
        bool apMode = controller->isApConnection;
        lv_color_t wifiColor = wifiConnected || apMode ? UI_COLOR_STANDBY_ICON_PRI : UI_COLOR_ICON_DISCONNECTED;
        lv_obj_set_style_img_recolor(ui_NewStandbyScreen_wifiIcon, wifiColor, LV_PART_MAIN);

        // BT icon state
        bool btConnected = !waitingForController;
        lv_color_t btColor = btConnected ? UI_COLOR_STANDBY_ICON_SEC : UI_COLOR_ICON_DISCONNECTED;
        lv_obj_set_style_img_recolor(ui_NewStandbyScreen_btIcon, btColor, LV_PART_MAIN);
        lv_obj_set_style_img_recolor_opa(ui_NewStandbyScreen_btIcon,
            btConnected ? UI_STANDBY_ICON_SEC_OPA : LV_OPA_COVER, LV_PART_MAIN);
    },
    &waitingForController
);
```

- [ ] **Step 6: Update initial screen on Bluetooth connect**

Change the bluetooth connect handler (around line 156-163) to navigate to new brew screen:

```cpp
pluginManager->on("controller:bluetooth:connected", [this](const PluginEvent &event) {
    waitingForController = 0;
    changeScreen(&ui_NewBrewScreen, &ui_NewBrewScreen_screen_init);
    controller->setMode(MODE_BREW);
    rerender = true;
});
```

- [ ] **Step 7: Handle brew timer and phase updates**

Add to the loop or as a reactive effect — update timer and phase during active brewing:

Place this code inside a DefaultUI method (e.g., `updateNewBrewScreen()` called from `loop()`):

```cpp
// In updateNewBrewScreen() — called from DefaultUI::loop()
void DefaultUI::updateNewBrewScreen() {
    if (!isBrewing || currentScreen != ui_NewBrewScreen) return;

    auto process = controller->getProcess();
    if (process) {
        // Timer — calculate from process start timestamp
        unsigned long elapsed = (millis() - process->started) / 1000;
        char buf[8];
        snprintf(buf, sizeof(buf), "%lu:%02lu", elapsed / 60, elapsed % 60);
        lv_label_set_text(ui_NewBrewScreen_timerLabel, buf);

        // Phase — use process->currentPhase.name
        if (!process->currentPhase.name.isEmpty()) {
            lv_label_set_text(ui_NewBrewScreen_phaseLabel, process->currentPhase.name.c_str());
        }

        // Check completion
        if (process->isComplete() && !isBrewComplete) {
            isBrewComplete = true;
            brewCompleteTime = millis();
            ui_NewBrewScreen_set_complete();
        }
    }
}

// Auto-dismiss brew complete after 5 seconds
if (isBrewComplete && millis() - brewCompleteTime > UI_BREW_COMPLETE_DISMISS_MS) {
    isBrewComplete = false;
    isBrewing = false;
    ui_NewBrewScreen_set_idle();
}
```

- [ ] **Step 8: Handle screen transitions with animations**

Update `handleScreenChange()` to use appropriate animations. Add a `previousScreen` field to DefaultUI and store it in `changeScreen()` before updating `targetScreen`:

```cpp
// In DefaultUI.h, add to private section:
lv_obj_t *previousScreen = nullptr;

// In changeScreen(), before setting targetScreen:
previousScreen = currentScreen;

// In handleScreenChange(), replace LV_SCR_LOAD_ANIM_NONE with:
lv_scr_load_anim_t anim = LV_SCR_LOAD_ANIM_FADE_ON;
uint32_t duration = UI_ANIM_FADE_MS;

// Determine animation based on transition direction
if (previousScreen != nullptr) {
    if ((*targetScreen == ui_NewWaterScreen && previousScreen == ui_NewBrewScreen) ||
        (*targetScreen == ui_NewSteamScreen && previousScreen == ui_NewWaterScreen)) {
        anim = LV_SCR_LOAD_ANIM_MOVE_LEFT;
        duration = UI_ANIM_SLIDE_MS;
    } else if ((*targetScreen == ui_NewBrewScreen && previousScreen == ui_NewWaterScreen) ||
               (*targetScreen == ui_NewWaterScreen && previousScreen == ui_NewSteamScreen)) {
        anim = LV_SCR_LOAD_ANIM_MOVE_RIGHT;
        duration = UI_ANIM_SLIDE_MS;
    }
}

_ui_screen_change(targetScreen, anim, duration, 0, targetScreenInit);
```

- [ ] **Step 9: Verify build compiles and flash**

Run: `pio run -e amoled`
Expected: Full build succeeds. Flash to device and verify basic flow.

- [ ] **Step 10: Commit**

```bash
git add src/display/ui/default/DefaultUI.h src/display/ui/default/DefaultUI.cpp src/display/ui/default/lvgl/ui.h
git commit -m "feat(ui): wire new screens into DefaultUI with swipe navigation"
```

---

## Chunk 7: Error States and Polish

### Task 9: Add error overlay and low water warning

**Files:**
- Modify: `src/display/ui/default/DefaultUI.cpp`

- [ ] **Step 1: Add error overlay logic**

In DefaultUI::loop(), after screen updates, check for error state:

```cpp
if (error && currentScreen != ui_InitScreen) {
    // Create/update error overlay
    // Map error code to text
    static const char* errorMessages[] = {
        "",                // 0 = NONE
        "COMM ERROR",     // 1 = COMM_SEND
        "COMM ERROR",     // 2 = COMM_RCV
        "PROTOCOL ERROR", // 3 = PROTO_ERR
        "THERMAL RUNAWAY",// 4 = RUNAWAY
        "CONNECTION LOST"  // 5 = TIMEOUT
    };
    int errCode = controller->getError();
    if (errCode > 0 && errCode <= 5) {
        // Show error overlay — create a semi-transparent red panel
        // Implementation: use a persistent overlay object, show/hide
    }
}
```

**Note:** The exact overlay implementation depends on LVGL's layer system. The simplest approach is a full-screen object with high z-index, red background at `UI_OPA(80)`, centered white text. Create it once in init, show/hide as needed.

- [ ] **Step 2: Add low water warning icon**

Similar approach — a small persistent warning object positioned at top-right of active screens, shown/hidden based on `controller->isLowWaterLevel()`.

- [ ] **Step 3: Commit**

```bash
git add src/display/ui/default/DefaultUI.cpp
git commit -m "feat(ui): add error overlay and low water warning"
```

### Task 10: Add auto-standby timeout

**Files:**
- Modify: `src/display/ui/default/DefaultUI.cpp`

- [ ] **Step 1: Add inactivity timer logic**

The existing `updateStandbyScreen()` already handles brightness dimming. Extend it or add a new check in `loop()`:

```cpp
// Track last interaction time
static unsigned long lastInteraction = millis();

// Reset on any rerender trigger (which happens on touch events)
if (rerender) {
    lastInteraction = millis();
}

// Check timeout (only when not in active operation and not already on standby)
if (currentScreen != ui_NewStandbyScreen &&
    !isBrewing && !isFlushing &&
    !controller->isActive() &&
    millis() - lastInteraction > settings.getStandbyTimeout()) {
    controller->activateStandby();
}
```

- [ ] **Step 2: Verify timeout doesn't trigger during operations**

Manually test: start brewing, wait > timeout, confirm screen doesn't switch to standby.

- [ ] **Step 3: Commit**

```bash
git add src/display/ui/default/DefaultUI.cpp
git commit -m "feat(ui): add configurable auto-standby timeout"
```

### Task 11: Final integration test on hardware

- [ ] **Step 1: Flash firmware to Waveshare 1.43" AMOLED**

Run: `pio run -e amoled -t upload`

- [ ] **Step 2: Verify full flow**

Walk through the complete interaction flow from the spec:
1. Power on → Standby (black, WiFi + BT icons)
2. Tap → Brew (rings, flush/brew buttons)
3. Tap Flush → Flushing state (blue glow, dimmed rings)
4. Tap Flush again → Stop, back to idle
5. Tap Brew → Active brewing (timer, phase, stop)
6. Tap Stop → Back to idle
7. Swipe right → Hot Water (blue ring, pump button)
8. Tap Pump → Water dispensing
9. Swipe right → Steam (red ring, no button)
10. Wait for temp → Ring turns green
11. Swipe down → Standby
12. Wait 60s → Auto-standby from any screen

- [ ] **Step 3: Check icon scaling**

Verify the WiFi/BT icons at 64px are not blurry from upscaling. If they are, create dedicated 64px icon assets as a follow-up task.

- [ ] **Step 4: Check font readability**

Verify Montserrat 10 labels are readable on the 1.43" display at arm's length. If too small, bump to Montserrat 12.

- [ ] **Step 5: Final commit if any adjustments needed**

```bash
git add -A
git commit -m "fix(ui): hardware testing adjustments"
```
