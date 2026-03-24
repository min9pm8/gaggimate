# Display Color Themes

## Overview

Add selectable color themes to the Waveshare 1.43" AMOLED display. Themes swap the three accent colors (primary, secondary, warning) while keeping the black AMOLED background, surface colors, text colors, and red error/stop color fixed.

## Themes

Six preset palettes:

| Theme | Primary | Secondary | Warning |
|-------|---------|-----------|---------|
| Default | `#00E676` (green) | `#448AFF` (blue) | `#FFAB40` (amber) |
| Cyan Frost | `#00E5FF` (cyan) | `#B388FF` (violet) | `#FFD180` (peach) |
| Ember | `#FF6D00` (orange) | `#FFD740` (gold) | `#FFAB91` (salmon) |
| Sakura | `#FF80AB` (pink) | `#CE93D8` (lavender) | `#FFCC80` (apricot) |
| Mint | `#1DE9B6` (teal) | `#B2FF59` (lime) | `#FFE57F` (lemon) |
| Neon | `#FF4081` (magenta) | `#40C4FF` (electric blue) | `#FFFF00` (yellow) |

### Fixed colors (all themes)

- Background: `#000000`
- Surface: `#1A1A1A`
- Text primary: `#E0E0E0`
- Text secondary: `#666666`
- Text tertiary: `#555555`
- Muted: `#333333`
- Button border: `#666666`
- Red/error: `#FF3B30`

## Behavior

- **Auto-rotate (default)**: On each boot, a random theme is selected and applied.
- **Manual selection**: User picks a specific theme in the web UI. That theme persists across reboots.
- **Web UI applies immediately**: Selecting a theme in the web UI applies it to the display in real-time.

## Settings

A single integer preference `colorTheme`, stored with key `"clr_theme"` in ESP32 Preferences namespace `"controller"`:

| Value | Meaning |
|-------|---------|
| 0 | Auto-rotate (random on boot) — default |
| 1 | Default |
| 2 | Cyan Frost |
| 3 | Ember |
| 4 | Sakura |
| 5 | Mint |
| 6 | Neon |

`setColorTheme()` must clamp values to 0–6 to prevent out-of-bounds palette access. Uses the existing deferred `save()` pattern (dirty flag + task) to avoid excessive flash writes.

## Architecture

### Color resolution

Replace the single-value `#define` accents in `ui_new_colors.h` with a theme palette struct and lookup:

```cpp
struct ColorThemePalette {
    lv_color_t primary;   // replaces UI_COLOR_GREEN
    lv_color_t secondary; // replaces UI_COLOR_BLUE
    lv_color_t warning;   // replaces UI_COLOR_AMBER
};
```

A `constexpr` array of 6 palettes (0-indexed internally). A global accessor function with C linkage returns the active palette, so all UI code — including the `.c` screen files — resolves colors at runtime:

```cpp
extern "C" const ColorThemePalette* ui_get_active_theme(void);
```

The existing `#define` macros become function-call macros:

```cpp
#define UI_COLOR_GREEN  (ui_get_active_theme()->primary)
#define UI_COLOR_BLUE   (ui_get_active_theme()->secondary)
#define UI_COLOR_AMBER  (ui_get_active_theme()->warning)
```

This means `.c` screen files (`ui_UnifiedScreen.c`, `ui_NewBrewScreen.c`, `ui_NewWaterScreen.c`) do NOT need source changes — their `lv_obj_set_style_*()` calls using these macros will resolve to the active palette at call time.

### Accent color reapply strategy

Colors are set in two places, requiring different reapply approaches:

1. **Screen init files (`.c`)** — Colors baked in during `screen_init()` at boot. These become stale after a runtime theme change.
2. **Render loop (`DefaultUI.cpp`)** — Colors set in `updateUnifiedScreen()`, `updateNewBrewScreen()`, etc. These auto-resolve on next loop iteration since the macros are now function calls.

For runtime theme changes, use a **re-init approach**: after changing the active palette, re-invoke `screen_init()` for the currently active screen. This is the simplest approach and acceptable because theme changes are infrequent user actions, not per-frame operations. The LVGL objects are recreated with the correct colors.

### Boot sequence

In `DefaultUI::setup()` or `applyTheme()`:

1. Read `colorTheme` from settings.
2. If 0 (auto): pick a random index 1–6 using `esp_random() % 6 + 1`.
3. Set the active palette to the selected theme.
4. Screen init proceeds normally — macros resolve to the active palette.

### Runtime theme switching

When the web UI sends a new `colorTheme` value:

1. `WebUIPlugin` receives the setting and calls `settings->setColorTheme(value)`.
2. Settings fires the existing `"settings:changed"` event (consistent with other settings).
3. `DefaultUI` handles `"settings:changed"`, compares current vs new `colorTheme`, and if changed: updates the active palette, re-inits the current screen.

### Interaction with Dark/Light theme (`themeMode`)

The `themeMode` (Dark/Light) and `colorTheme` are orthogonal. `themeMode` controls background polarity via `ui_theme_set()`. Color themes control accent colors via the palette accessor. Order matters: apply `ui_theme_set()` first (which may set default accent styles), then apply the color theme palette (which overrides accents). This ensures the color theme always wins for accent colors.

### Web UI

In `web/src/pages/Settings/index.jsx`, add a dropdown in the Display section:

```
Color Theme: [Auto (Random) | Default | Cyan Frost | Ember | Sakura | Mint | Neon]
```

The selected value is sent as `colorTheme` in the settings form POST to `/api/settings`. The GET endpoint must also include `colorTheme` in its JSON response so the dropdown reflects the current setting on page load.

## Files to modify

| File | Change |
|------|--------|
| `src/display/ui/default/lvgl/ui_new_colors.h` | Add `ColorThemePalette` struct, palette array, C-linkage accessor. Redefine `UI_COLOR_GREEN`/`UI_COLOR_BLUE`/`UI_COLOR_AMBER` as function-call macros. |
| `src/display/core/Settings.h` | Add `getColorTheme()` / `setColorTheme(int)` with clamping |
| `src/display/core/Settings.cpp` | Persist `"clr_theme"` preference, default 0 |
| `src/display/ui/default/DefaultUI.cpp` | Apply theme on boot (random or fixed), handle `"settings:changed"` for runtime reapply, re-init active screen on theme change |
| `src/display/plugins/WebUIPlugin.cpp` | Handle `colorTheme` param in settings POST, include in settings GET response |
| `web/src/pages/Settings/index.jsx` | Add color theme dropdown |

## Constraints

- No heap allocation for palette storage — use `constexpr` arrays.
- Keep the existing Dark/Light theme toggle (`themeMode`) independent — it controls background polarity, this controls accent colors.
- Red (`UI_COLOR_RED`) must never be overridden by a theme.
- Input validation: clamp `colorTheme` to 0–6.
