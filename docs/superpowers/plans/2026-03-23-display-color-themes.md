# Display Color Themes Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add 6 selectable AMOLED accent color themes with auto-rotate-on-boot default, configurable from the web UI.

**Architecture:** Replace compile-time color `#define` macros with function-call macros that resolve from a global active palette. Settings stores theme choice (0=auto, 1-6=specific). On boot, auto selects random; on web UI change, force screen re-init to reapply colors.

**Tech Stack:** C/C++ (ESP32/LVGL 8.x), Preact (web UI), ESP32 Preferences (NVS)

**Spec:** `docs/superpowers/specs/2026-03-23-display-color-themes-design.md`

---

### Task 1: Add ColorThemePalette and palette array to ui_new_colors.h

**Files:**
- Modify: `src/display/ui/default/lvgl/ui_new_colors.h`
- Create: `src/display/ui/default/lvgl/ui_color_themes.c`

- [ ] **Step 1: Replace the accent color `#define` block in ui_new_colors.h**

Replace lines 24-28 (the `// === Accent Colors ===` section) with:

```c
// === Accent Colors (theme-resolved at runtime) ===
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    lv_color_t primary;   // green role
    lv_color_t secondary; // blue role
    lv_color_t warning;   // amber role
} ColorThemePalette;

#define UI_COLOR_THEME_COUNT 6

const ColorThemePalette* ui_get_active_theme(void);
void ui_set_active_theme(int index);  // 0-based index (0-5)

#ifdef __cplusplus
}
#endif

#define UI_COLOR_GREEN  (ui_get_active_theme()->primary)
#define UI_COLOR_BLUE   (ui_get_active_theme()->secondary)
#define UI_COLOR_AMBER  (ui_get_active_theme()->warning)
```

Keep `UI_COLOR_RED` unchanged on its own line:
```c
#define UI_COLOR_RED               lv_color_hex(0xFF3B30)
```

- [ ] **Step 2: Create the palette implementation file**

Create `src/display/ui/default/lvgl/ui_color_themes.c`. This build uses `LV_COLOR_DEPTH 16` (RGB565), so `lv_color_t.full` is `uint16_t` — we must use `lv_color_hex()` to convert 24-bit hex to RGB565:

```c
#include "ui_new_colors.h"
#include <stdbool.h>

static ColorThemePalette palettes[UI_COLOR_THEME_COUNT];
static bool palettes_initialized = false;
static int active_theme_index = 0;

static void init_palettes(void) {
    if (palettes_initialized) return;
    // 0: Default
    palettes[0] = (ColorThemePalette){ lv_color_hex(0x00E676), lv_color_hex(0x448AFF), lv_color_hex(0xFFAB40) };
    // 1: Cyan Frost
    palettes[1] = (ColorThemePalette){ lv_color_hex(0x00E5FF), lv_color_hex(0xB388FF), lv_color_hex(0xFFD180) };
    // 2: Ember
    palettes[2] = (ColorThemePalette){ lv_color_hex(0xFF6D00), lv_color_hex(0xFFD740), lv_color_hex(0xFFAB91) };
    // 3: Sakura
    palettes[3] = (ColorThemePalette){ lv_color_hex(0xFF80AB), lv_color_hex(0xCE93D8), lv_color_hex(0xFFCC80) };
    // 4: Mint
    palettes[4] = (ColorThemePalette){ lv_color_hex(0x1DE9B6), lv_color_hex(0xB2FF59), lv_color_hex(0xFFE57F) };
    // 5: Neon
    palettes[5] = (ColorThemePalette){ lv_color_hex(0xFF4081), lv_color_hex(0x40C4FF), lv_color_hex(0xFFFF00) };
    palettes_initialized = true;
}

const ColorThemePalette* ui_get_active_theme(void) {
    init_palettes();
    return &palettes[active_theme_index];
}

void ui_set_active_theme(int index) {
    if (index >= 0 && index < UI_COLOR_THEME_COUNT) {
        init_palettes();
        active_theme_index = index;
    }
}
```

Note: No heap allocation. Static arrays, initialized lazily because `lv_color_hex()` is not constexpr.

- [ ] **Step 3: Verify build compiles**

Run: `pio run -e waveshare-1_43-amoled`
Expected: Compiles successfully. The existing `.c` screen files resolve `UI_COLOR_GREEN`/`UI_COLOR_BLUE`/`UI_COLOR_AMBER` through the new function-call macros with no source changes. PlatformIO auto-discovers the new `.c` file via `build_src_filter = +<display/>`.

- [ ] **Step 4: Commit**

```bash
git add src/display/ui/default/lvgl/ui_new_colors.h src/display/ui/default/lvgl/ui_color_themes.c
git commit -m "feat(ui): add color theme palette system with 6 presets"
```

---

### Task 2: Add colorTheme to Settings

**Files:**
- Modify: `src/display/core/Settings.h`
- Modify: `src/display/core/Settings.cpp`

- [ ] **Step 1: Add getter/setter declaration in Settings.h**

After `int getThemeMode() const { return themeMode; }` (line 101), add:
```cpp
int getColorTheme() const { return colorTheme; }
```

After `void setThemeMode(int theme_mode);` (line 164), add:
```cpp
void setColorTheme(int color_theme);
```

In the private section, after `int themeMode = 0;` (line 236), add:
```cpp
int colorTheme = 0; // 0 = auto-rotate, 1-6 = specific theme
```

- [ ] **Step 2: Add persistence in Settings.cpp constructor**

After `themeMode = preferences.getInt("theme", 0);` (line 96), add:
```cpp
colorTheme = preferences.getInt("clr_theme", 0);
```

- [ ] **Step 3: Add setter in Settings.cpp**

After `setThemeMode` (lines 374-377), add:
```cpp
void Settings::setColorTheme(int color_theme) {
    colorTheme = std::clamp(color_theme, 0, 6);
    save();
}
```

- [ ] **Step 4: Add to doSave()**

After `preferences.putInt("theme", themeMode);` (line 526), add:
```cpp
preferences.putInt("clr_theme", colorTheme);
```

- [ ] **Step 5: Verify build compiles**

Run: `pio run -e waveshare-1_43-amoled`
Expected: Compiles successfully.

- [ ] **Step 6: Commit**

```bash
git add src/display/core/Settings.h src/display/core/Settings.cpp
git commit -m "feat(settings): add colorTheme preference (0=auto, 1-6=preset)"
```

---

### Task 3: Apply color theme on boot and handle runtime changes in DefaultUI

**Files:**
- Modify: `src/display/ui/default/DefaultUI.h`
- Modify: `src/display/ui/default/DefaultUI.cpp`

- [ ] **Step 1: Add member variable and helper declaration to DefaultUI.h**

After `int currentThemeMode = -1;` (line 123), add:
```cpp
int currentColorTheme = -1; // Force apply on first loop
```

In the private method declarations (near `changeScreen` at line 34), add:
```cpp
void forceReinitScreen(lv_obj_t **screen, void (*target_init)(void));
```

- [ ] **Step 2: Add include for esp_random**

At the top of `DefaultUI.cpp`, add (if not already present):
```cpp
#include <esp_random.h>
```

- [ ] **Step 3: Add forceReinitScreen helper to DefaultUI.cpp**

After the `changeScreen` method (line 394-402), add:

```cpp
void DefaultUI::forceReinitScreen(lv_obj_t **screen, void (*target_init)(void)) {
    // Force screen re-creation by nulling the pointer.
    // handleScreenChange/changeScreen skip re-init when the target is already active.
    // _ui_screen_change only calls target_init() when *target == NULL.
    // So we must: delete the old LVGL object, null the pointer, then trigger changeScreen.
    lv_obj_t *old = *screen;
    *screen = NULL;
    changeScreen(screen, target_init);
    // handleScreenChange will see current != *targetScreen (NULL) and call
    // _ui_screen_change which calls target_init() since *target == NULL,
    // then deletes the old screen object.
}
```

- [ ] **Step 4: Extend applyTheme() in DefaultUI.cpp**

Replace the `applyTheme()` method (lines 1245-1257) with:

```cpp
void DefaultUI::applyTheme() {
    const Settings &settings = controller->getSettings();
    int newThemeMode = settings.getThemeMode();

    if (newThemeMode != currentThemeMode) {
        currentThemeMode = newThemeMode;
        ui_theme_set(currentThemeMode);

        if (AmoledDisplayDriver::getInstance() == panelDriver && currentThemeMode == UI_THEME_DEFAULT) {
            enable_amoled_black_theme_override(lv_disp_get_default());
        }
    }

    // Color theme (accent palette)
    int newColorTheme = settings.getColorTheme();
    if (newColorTheme != currentColorTheme) {
        if (newColorTheme == 0) {
            // Auto-rotate: pick random theme
            ui_set_active_theme(esp_random() % UI_COLOR_THEME_COUNT);
        } else {
            // Manual: settings value 1-6 maps to palette index 0-5
            ui_set_active_theme(newColorTheme - 1);
        }
        currentColorTheme = newColorTheme;

        // Re-init current screen to reapply accent colors baked during screen_init.
        // Skip on first boot (currentScreen not yet set — screen_init will use new palette).
        if (currentScreen != nullptr) {
            if (currentScreen == ui_UnifiedScreen)
                forceReinitScreen(&ui_UnifiedScreen, &ui_UnifiedScreen_screen_init);
            else if (currentScreen == ui_NewBrewScreen)
                forceReinitScreen(&ui_NewBrewScreen, &ui_NewBrewScreen_screen_init);
            else if (currentScreen == ui_NewWaterScreen)
                forceReinitScreen(&ui_NewWaterScreen, &ui_NewWaterScreen_screen_init);
            else if (currentScreen == ui_NewSteamScreen)
                forceReinitScreen(&ui_NewSteamScreen, &ui_NewSteamScreen_screen_init);
            else if (currentScreen == ui_NewStandbyScreen)
                forceReinitScreen(&ui_NewStandbyScreen, &ui_NewStandbyScreen_screen_init);
        }
    }
}
```

- [ ] **Step 5: Verify build compiles**

Run: `pio run -e waveshare-1_43-amoled`
Expected: Compiles successfully.

- [ ] **Step 6: Commit**

```bash
git add src/display/ui/default/DefaultUI.h src/display/ui/default/DefaultUI.cpp
git commit -m "feat(ui): apply color theme on boot with auto-rotate, handle runtime changes"
```

---

### Task 4: Wire colorTheme through WebUIPlugin API

**Files:**
- Modify: `src/display/plugins/WebUIPlugin.cpp`

- [ ] **Step 1: Add colorTheme to settings POST handler**

After the `themeMode` handling (lines 555-556), add:
```cpp
if (request->hasArg("colorTheme"))
    settings->setColorTheme(request->arg("colorTheme").toInt());
```

- [ ] **Step 2: Add colorTheme to settings GET response**

After `doc["themeMode"] = settings.getThemeMode();` (line 661), add:
```cpp
doc["colorTheme"] = settings.getColorTheme();
```

- [ ] **Step 3: Verify build compiles**

Run: `pio run -e waveshare-1_43-amoled`
Expected: Compiles successfully.

- [ ] **Step 4: Commit**

```bash
git add src/display/plugins/WebUIPlugin.cpp
git commit -m "feat(api): expose colorTheme in settings GET/POST"
```

---

### Task 5: Add color theme dropdown to web UI Settings page

**Files:**
- Modify: `web/src/pages/Settings/index.jsx`

- [ ] **Step 1: Add colorTheme dropdown after the existing Theme select**

After the `themeMode` form-control closing `</div>` (line 862), add:

```jsx
<div className='form-control'>
  <label htmlFor='colorTheme' className='mb-2 block text-sm font-medium'>
    Color Theme
  </label>
  <select
    id='colorTheme'
    name='colorTheme'
    className='select select-bordered w-full'
    value={formData.colorTheme}
    onChange={onChange('colorTheme')}
  >
    <option value={0}>Auto (Random)</option>
    <option value={1}>Default</option>
    <option value={2}>Cyan Frost</option>
    <option value={3}>Ember</option>
    <option value={4}>Sakura</option>
    <option value={5}>Mint</option>
    <option value={6}>Neon</option>
  </select>
</div>
```

- [ ] **Step 2: Build web UI to verify**

Run: `cd web && npm run build`
Expected: Builds without errors.

- [ ] **Step 3: Commit**

```bash
git add web/src/pages/Settings/index.jsx
git commit -m "feat(web): add color theme dropdown to display settings"
```

---

### Task 6: Full build verification

**Files:** None (verification only)

- [ ] **Step 1: Build the waveshare target**

Run: `pio run -e waveshare-1_43-amoled`
Expected: Compiles with no errors or warnings related to color theme changes.

- [ ] **Step 2: Build web UI**

Run: `cd web && npm run build`
Expected: Builds successfully.

- [ ] **Step 3: Verify no regressions in other build environments**

Run: `pio run` (builds all configured environments)
Expected: All environments compile successfully. The function-call macros work from both C and C++ translation units.
