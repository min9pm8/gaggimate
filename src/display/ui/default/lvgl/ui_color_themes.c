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
