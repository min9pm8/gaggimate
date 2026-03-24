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

A single integer preference `colorTheme`:

| Value | Meaning |
|-------|---------|
| 0 | Auto-rotate (random on boot) — default |
| 1 | Default |
| 2 | Cyan Frost |
| 3 | Ember |
| 4 | Sakura |
| 5 | Mint |
| 6 | Neon |

Stored in ESP32 Preferences namespace `"controller"` with key `"colorTheme"`.

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

A `constexpr` array of 6 palettes, indexed by theme ID (1-based, subtract 1 for array index). A global accessor function returns the active palette, so all UI code resolves colors at runtime instead of compile time.

### Boot sequence

In `DefaultUI::setup()` or `applyTheme()`:

1. Read `colorTheme` from settings.
2. If 0 (auto): pick a random index 1–6 using `esp_random() % 6 + 1`.
3. Set the active palette to the selected theme.
4. Apply colors to all LVGL objects that reference accent colors.

### Runtime theme switching

When the web UI sends a new `colorTheme` value:

1. `WebUIPlugin` receives the setting and calls `settings->setColorTheme(value)`.
2. Fire an event `"settings:colorTheme:change"`.
3. `DefaultUI` listens for that event, updates the active palette, and reapplies accent colors to all live LVGL objects.

### Web UI

In `web/src/pages/Settings/index.jsx`, add a dropdown in the Display section:

```
Color Theme: [Auto (Random) | Default | Cyan Frost | Ember | Sakura | Mint | Neon]
```

The selected value is sent as `colorTheme` in the settings form POST to `/api/settings`.

## Files to modify

| File | Change |
|------|--------|
| `src/display/ui/default/lvgl/ui_new_colors.h` | Add `ColorThemePalette` struct, palette array, accessor function. Keep `#define` macros as aliases to the active palette for backward compat. |
| `src/display/core/Settings.h` | Add `getColorTheme()` / `setColorTheme(int)` |
| `src/display/core/Settings.cpp` | Persist `"colorTheme"` preference, default 0 |
| `src/display/ui/default/DefaultUI.cpp` | Apply theme on boot (random or fixed), listen for runtime changes, reapply accent colors |
| `src/display/plugins/WebUIPlugin.cpp` | Handle `colorTheme` param in settings API, emit change event |
| `web/src/pages/Settings/index.jsx` | Add color theme dropdown |

## Constraints

- No heap allocation for palette storage — use `constexpr` arrays.
- Keep the existing Dark/Light theme toggle (`themeMode`) independent — it controls background polarity, this controls accent colors. They are orthogonal.
- Red (`UI_COLOR_RED`) must never be overridden by a theme.
