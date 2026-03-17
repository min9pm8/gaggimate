#include "DefaultUI.h"

#include <WiFi.h>
#include <display/config.h>
#include <display/core/Controller.h>
#include <display/core/process/BrewProcess.h>
#include <display/core/process/Process.h>
#include <display/core/zones.h>
#include <display/drivers/AmoledDisplayDriver.h>
#include <display/drivers/LilyGoDriver.h>
#include <display/drivers/WaveshareDriver.h>
#include <display/drivers/common/LV_Helper.h>
#include <display/ui/default/lvgl/ui_theme_manager.h>
#include <display/ui/default/lvgl/ui_themes.h>
#include <display/ui/utils/effects.h>

#include "esp_sntp.h"

static EffectManager effect_mgr;

int16_t calculate_angle(int set_temp, int range, int offset) {
    const double percentage = static_cast<double>(set_temp) / static_cast<double>(MAX_TEMP);
    return (percentage * ((double)range)) - range / 2 - offset;
}

void DefaultUI::updateTempHistory() {
    if (currentTemp > 0) {
        tempHistory[tempHistoryIndex] = currentTemp;
        tempHistoryIndex += 1;
    }

    if (tempHistoryIndex > TEMP_HISTORY_LENGTH) {
        tempHistoryIndex = 0;
        isTempHistoryInitialized = true;
    }

    if (tempHistoryIndex % 4 == 0) {
        heatingFlash = !heatingFlash;
        rerender = true;
    }
}

void DefaultUI::updateTempStableFlag() {
    if (isTempHistoryInitialized) {
        float totalError = 0.0f;
        float maxError = 0.0f;
        for (uint16_t i = 0; i < TEMP_HISTORY_LENGTH; i++) {
            float error = abs(tempHistory[i] - targetTemp);
            totalError += error;
            maxError = error > maxError ? error : maxError;
        }

        const float avgError = totalError / TEMP_HISTORY_LENGTH;
        const float errorMargin = max(2.0f, static_cast<float>(targetTemp) * 0.02f);

        isTemperatureStable = avgError < errorMargin && maxError <= errorMargin;
    }

    // instantly reset stability if setpoint has changed
    if (prevTargetTemp != targetTemp) {
        isTemperatureStable = false;
    }

    prevTargetTemp = targetTemp;
}

void DefaultUI::adjustHeatingIndicator(lv_obj_t *dials) {
    lv_obj_t *heatingIcon = ui_comp_get_child(dials, UI_COMP_DIALS_TEMPICON);
    lv_obj_set_style_img_recolor(heatingIcon, lv_color_hex(isTemperatureStable ? 0x00D100 : 0xF62C2C),
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
    if (!isTemperatureStable) {
        lv_obj_set_style_opa(heatingIcon, heatingFlash ? LV_OPA_50 : LV_OPA_100, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

DefaultUI::DefaultUI(Controller *controller, Driver *driver, PluginManager *pluginManager)
    : controller(controller), panelDriver(driver), pluginManager(pluginManager) {
    setupPanel();
}

void DefaultUI::init() {
    profileManager = controller->getProfileManager();
    auto triggerRender = [this](Event const &) { rerender = true; };
    pluginManager->on("boiler:currentTemperature:change", [=](Event const &event) {
        int newTemp = static_cast<int>(event.getFloat("value"));
        if (newTemp != currentTemp) {
            currentTemp = newTemp;
            rerender = true;
        }
    });
    pluginManager->on("boiler:pressure:change", [=](Event const &event) {
        float newPressure = event.getFloat("value");
        if (round(newPressure * 10.0f) != round(pressure * 10.0f)) {
            pressure = newPressure;
            rerender = true;
        }
    });
    pluginManager->on("boiler:targetTemperature:change", [=](Event const &event) {
        int newTemp = static_cast<int>(event.getFloat("value"));
        if (newTemp != targetTemp) {
            targetTemp = newTemp;
            rerender = true;
        }
    });
    pluginManager->on("controller:targetVolume:change", [=](Event const &event) {
        targetVolume = event.getFloat("value");
        rerender = true;
    });
    pluginManager->on("controller:targetDuration:change", [=](Event const &event) {
        targetDuration = event.getFloat("value");
        rerender = true;
    });
    pluginManager->on("controller:grindDuration:change", [=](Event const &event) {
        grindDuration = event.getInt("value");
        rerender = true;
    });
    pluginManager->on("controller:grindVolume:change", [=](Event const &event) {
        grindVolume = event.getFloat("value");
        rerender = true;
    });
    pluginManager->on("controller:process:end", triggerRender);
    pluginManager->on("controller:process:start", triggerRender);
    pluginManager->on("controller:mode:change", [this](Event const &event) {
        mode = event.getInt("value");
        switch (mode) {
        case MODE_STANDBY:
            if (currentScreen == ui_UnifiedScreen && ui_UnifiedScreen_tempLabel != NULL) {
                ui_UnifiedScreen_set_mode_standby();
            } else {
                changeScreen(&ui_NewStandbyScreen, &ui_NewStandbyScreen_screen_init);
            }
            break;
        case MODE_BREW:
        case MODE_GRIND:
            if (currentScreen == ui_UnifiedScreen && ui_UnifiedScreen_tempLabel != NULL) {
                ui_UnifiedScreen_set_mode_brew();
            } else {
                // screen_init defaults to brew mode — set_mode_brew called after init in handleScreenChange
                changeScreen(&ui_UnifiedScreen, &ui_UnifiedScreen_screen_init);
            }
            break;
        case MODE_STEAM:
            if (currentScreen == ui_UnifiedScreen && ui_UnifiedScreen_tempLabel != NULL) {
                ui_UnifiedScreen_set_mode_steam();
            } else {
                changeScreen(&ui_UnifiedScreen, &ui_UnifiedScreen_screen_init);
            }
            break;
        case MODE_WATER:
            if (currentScreen == ui_UnifiedScreen && ui_UnifiedScreen_tempLabel != NULL) {
                ui_UnifiedScreen_set_mode_water();
            } else {
                changeScreen(&ui_UnifiedScreen, &ui_UnifiedScreen_screen_init);
            }
            break;
        default:
            break;
        };
    });
    pluginManager->on("controller:brew:start", [this](Event const &event) {
        isBrewing = true;
        isBrewComplete = false;
        if (lv_scr_act() == ui_NewBrewScreen) {
            ui_NewBrewScreen_set_brewing();
        }
        if (lv_scr_act() == ui_UnifiedScreen && ui_UnifiedScreen_tempLabel != NULL) {
            ui_UnifiedScreen_set_brewing();
        }
    });
    pluginManager->on("controller:brew:clear", [this](Event const &event) {
        if (isBrewing && !isBrewComplete) {
            isBrewing = false;
            if (lv_scr_act() == ui_NewBrewScreen) {
                ui_NewBrewScreen_set_idle();
            }
            if (lv_scr_act() == ui_UnifiedScreen && ui_UnifiedScreen_tempLabel != NULL) {
                ui_UnifiedScreen_set_idle();
            }
        }
    });
    pluginManager->on("controller:bluetooth:waiting", [this](Event const &) {
        waitingForController = true;
        rerender = true;
    });
    pluginManager->on("controller:bluetooth:connect", [this](Event const &) {
        waitingForController = false;
        rerender = true;
        if (lv_scr_act() == ui_InitScreen) {
            Settings &settings = controller->getSettings();
            if (settings.getStartupMode() == MODE_BREW) {
                changeScreen(&ui_UnifiedScreen, &ui_UnifiedScreen_screen_init);
            } else {
                changeScreen(&ui_NewStandbyScreen, &ui_NewStandbyScreen_screen_init);
            }
        }
        pressureAvailable = controller->getSystemInfo().capabilities.pressure;
    });
    pluginManager->on("controller:wifi:connect", [this](Event const &event) {
        rerender = true;
        apActive = event.getInt("AP");
    });
    pluginManager->on("ota:update:start", [this](Event const &) {
        updateActive = true;
        rerender = true;
        changeScreen(&ui_InitScreen, &ui_InitScreen_screen_init);
    });
    pluginManager->on("ota:update:end", [this](Event const &) {
        updateActive = false;
        rerender = true;
        changeScreen(&ui_InitScreen, &ui_InitScreen_screen_init);
    });
    pluginManager->on("ota:update:status", [this](Event const &event) {
        rerender = true;
        updateAvailable = event.getInt("value");
    });
    pluginManager->on("controller:error", [this](Event const &) {
        rerender = true;
        changeScreen(&ui_InitScreen, &ui_InitScreen_screen_init);
    });
    pluginManager->on("controller:autotune:start",
                      [this](Event const &) { changeScreen(&ui_InitScreen, &ui_InitScreen_screen_init); });
    pluginManager->on("controller:autotune:result",
                      [this](Event const &) { changeScreen(&ui_StandbyScreen, &ui_StandbyScreen_screen_init); });

    pluginManager->on("profiles:profile:select", [this](Event const &event) {
        profileManager->loadSelectedProfile(selectedProfile);
        selectedProfileId = event.getString("id");
        targetDuration = profileManager->getSelectedProfile().getTotalDuration();
        targetVolume = profileManager->getSelectedProfile().getTotalVolume();
        rerender = true;
    });
    pluginManager->on("controller:volumetric-measurement:bluetooth:change", [=](Event const &event) {
        double newWeight = event.getFloat("value");
        if (round(newWeight * 10.0) != round(bluetoothWeight * 10.0)) {
            bluetoothWeight = newWeight;
            rerender = true;
        }
    });
    setupState();
    setupReactive();
    xTaskCreatePinnedToCore(loopTask, "DefaultUI::loop", configMINIMAL_STACK_SIZE * 6, this, 1, &taskHandle, 1);
    xTaskCreatePinnedToCore(profileLoopTask, "DefaultUI::loopProfiles", configMINIMAL_STACK_SIZE * 4, this, 1, &profileTaskHandle,
                            0);
}

void DefaultUI::loop() {
    const unsigned long now = millis();
    const unsigned long diff = now - lastRender;

    if (now - lastTempLog > TEMP_HISTORY_INTERVAL) {
        updateTempHistory();
        lastTempLog = now;
    }

    if ((controller->isActive() && diff > RERENDER_INTERVAL_ACTIVE) || diff > RERENDER_INTERVAL_IDLE) {
        rerender = true;
    }

    // Capture rerender flag before it is cleared, to track last interaction time
    const bool hadRerender = rerender;

    if (rerender) {
        rerender = false;
        lastRender = now;
        error = controller->isErrorState();
        autotuning = controller->isAutotuning();
        const Settings &settings = controller->getSettings();
        volumetricAvailable = controller->isVolumetricAvailable();
        bluetoothScales = controller->isBluetoothScaleHealthy();
        volumetricMode = volumetricAvailable && settings.isVolumetricTarget();
        grindActive = controller->isGrindActive();
        active = controller->isActive();
        smartGrindActive = settings.isSmartGrindActive();
        grindAvailable = smartGrindActive || settings.getAltRelayFunction() == ALT_RELAY_GRIND;
        applyTheme();
        if (controller->isErrorState()) {
            changeScreen(&ui_InitScreen, &ui_InitScreen_screen_init);
        }
        updateTempStableFlag();
        handleScreenChange();
        currentScreen = lv_scr_act();
        if (lv_scr_act() == ui_StandbyScreen)
            updateStandbyScreen();
        if (lv_scr_act() == ui_StatusScreen)
            updateStatusScreen();
        updateNewBrewScreen();
        updateUnifiedScreen();
        updateNewStandbyScreen();

        // Update clock on unified screen
        if (currentScreen == ui_UnifiedScreen && ui_UnifiedScreen_clockLabel != NULL) {
            time_t now = time(NULL);
            if (now > 1000000000) { // NTP has synced (timestamp is reasonable)
                struct tm timeinfo;
                localtime_r(&now, &timeinfo);
                char timeStr[12];
                Settings &settings = controller->getSettings();
                const char *format = settings.isClock24hFormat() ? "%H:%M" : "%I:%M %p";
                strftime(timeStr, sizeof(timeStr), format, &timeinfo);
                lv_label_set_text(ui_UnifiedScreen_clockLabel, timeStr);
            }
        }

        effect_mgr.evaluate_all();
    }

    // Error overlay management
    static lv_obj_t *errorOverlay = NULL;
    if (error && currentScreen != ui_InitScreen) {
        if (errorOverlay == NULL || lv_obj_get_parent(errorOverlay) != lv_scr_act()) {
            // Clean up old overlay if screen changed
            if (errorOverlay != NULL) {
                lv_obj_del(errorOverlay);
            }
            errorOverlay = lv_obj_create(lv_scr_act());
            lv_obj_set_size(errorOverlay, 466, 466);
            lv_obj_center(errorOverlay);
            lv_obj_set_style_bg_color(errorOverlay, UI_COLOR_RED, LV_PART_MAIN);
            lv_obj_set_style_bg_opa(errorOverlay, UI_OPA(80), LV_PART_MAIN);
            lv_obj_set_style_radius(errorOverlay, LV_RADIUS_CIRCLE, LV_PART_MAIN);
            lv_obj_clear_flag(errorOverlay, LV_OBJ_FLAG_SCROLLABLE);

            lv_obj_t *errLabel = lv_label_create(errorOverlay);
            // Map error code to text
            static const char *errorMessages[] = {"", "COMM ERROR", "COMM ERROR", "PROTOCOL ERROR", "THERMAL RUNAWAY",
                                                   "CONNECTION LOST"};
            int errCode = controller->getError();
            const char *msg = (errCode > 0 && errCode <= 5) ? errorMessages[errCode] : "ERROR";
            lv_label_set_text(errLabel, msg);
            lv_obj_set_style_text_font(errLabel, &lv_font_montserrat_16, LV_PART_MAIN);
            lv_obj_set_style_text_color(errLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
            lv_obj_center(errLabel);
        }
    } else if (!error && errorOverlay != NULL) {
        lv_obj_del(errorOverlay);
        errorOverlay = NULL;
    }

    // Auto-standby timeout — track last interaction time (touch events trigger rerender)
    static unsigned long lastInteraction = millis();
    if (hadRerender) {
        lastInteraction = millis();
    }
    // Only auto-standby from active mode screens, not from init/waiting/already-standby screens
    bool alreadyStandby = (currentScreen == ui_NewStandbyScreen) ||
        (currentScreen == ui_UnifiedScreen && mode == MODE_STANDBY);
    if (!alreadyStandby &&
        currentScreen != ui_InitScreen &&
        !active && !waitingForController) {
        const Settings &autoStandbySettings = controller->getSettings();
        if (autoStandbySettings.getStandbyTimeout() > 0 &&
            millis() - lastInteraction > (unsigned long)autoStandbySettings.getStandbyTimeout()) {
            controller->activateStandby();
        }
    }

    lv_task_handler();
}

void DefaultUI::loopProfiles() {
    if (!profileLoaded && currentProfileId != "") {
        profileManager->loadProfile(currentProfileId, currentProfileChoice);
        profileLoaded = 1;
    }
}

void DefaultUI::changeScreen(lv_obj_t **screen, void (*target_init)()) {
    previousScreen = currentScreen;
    targetScreen = screen;
    targetScreenInit = target_init;
    rerender = true;

    // Reset some submenus
    brewScreenState = BrewScreenState::Brew;
}

void DefaultUI::changeBrewScreenMode(BrewScreenState state) {
    brewScreenState = state;
    rerender = true;
}

void DefaultUI::onProfileSwitch() {
    favoritedProfiles = profileManager->getFavoritedProfiles();
    currentProfileIdx = 0;
    currentProfileId = favoritedProfiles[currentProfileIdx];
    profileLoaded = 0;
    currentProfileChoice = Profile{};
    changeScreen(&ui_ProfileScreen, ui_ProfileScreen_screen_init);
}

void DefaultUI::onNextProfile() {
    if (currentProfileIdx < favoritedProfiles.size() - 1) {
        currentProfileIdx++;
        currentProfileId = favoritedProfiles.at(currentProfileIdx);
        profileLoaded = 0;
        currentProfileChoice = Profile{};
    }
}

void DefaultUI::onPreviousProfile() {
    if (currentProfileIdx > 0) {
        currentProfileIdx--;
        currentProfileId = favoritedProfiles.at(currentProfileIdx);
        profileLoaded = 0;
        currentProfileChoice = Profile{};
    }
}

void DefaultUI::onProfileSelect() {
    profileManager->selectProfile(currentProfileId);
    changeScreen(&ui_BrewScreen, ui_BrewScreen_screen_init);
}

void DefaultUI::setupPanel() {
    ui_init();
    lv_task_handler();

    delay(100);
    // Set initial brightness based on settings
    const Settings &settings = controller->getSettings();
    setBrightness(settings.getMainBrightness());
}

void DefaultUI::setupState() {
    error = controller->isErrorState();
    autotuning = controller->isAutotuning();
    const Settings &settings = controller->getSettings();
    volumetricAvailable = controller->isVolumetricAvailable();
    volumetricMode = volumetricAvailable && settings.isVolumetricTarget();
    grindActive = controller->isGrindActive();
    active = controller->isActive();
    smartGrindActive = settings.isSmartGrindActive();
    grindAvailable = smartGrindActive || settings.getAltRelayFunction() == ALT_RELAY_GRIND;
    mode = controller->getMode();
    currentTemp = static_cast<int>(controller->getCurrentTemp());
    targetTemp = static_cast<int>(controller->getTargetTemp());
    targetDuration = profileManager->getSelectedProfile().getTotalDuration();
    targetVolume = profileManager->getSelectedProfile().getTotalVolume();
    grindDuration = settings.getTargetGrindDuration();
    grindVolume = settings.getTargetGrindVolume();
    pressureAvailable = controller->getSystemInfo().capabilities.pressure ? 1 : 0;
    pressureScaling = std::ceil(settings.getPressureScaling());
    selectedProfileId = settings.getSelectedProfile();
    profileManager->loadSelectedProfile(selectedProfile);
}

void DefaultUI::setupReactive() {
    effect_mgr.use_effect([=] { return currentScreen == ui_MenuScreen; }, [=]() { adjustDials(ui_MenuScreen_dials); },
                          &pressureAvailable);
    effect_mgr.use_effect([=] { return currentScreen == ui_StatusScreen; }, [=]() { adjustDials(ui_StatusScreen_dials); },
                          &pressureAvailable);
    effect_mgr.use_effect([=] { return currentScreen == ui_BrewScreen; }, [=]() { adjustDials(ui_BrewScreen_dials); },
                          &pressureAvailable);
    effect_mgr.use_effect([=] { return currentScreen == ui_GrindScreen; }, [=]() { adjustDials(ui_GrindScreen_dials); },
                          &pressureAvailable);
    effect_mgr.use_effect([=] { return currentScreen == ui_SimpleProcessScreen; },
                          [=]() { adjustDials(ui_SimpleProcessScreen_dials); }, &pressureAvailable);
    effect_mgr.use_effect([=] { return currentScreen == ui_ProfileScreen; }, [=]() { adjustDials(ui_ProfileScreen_dials); },
                          &pressureAvailable);
    effect_mgr.use_effect([=] { return currentScreen == ui_BrewScreen; }, [=]() { adjustHeatingIndicator(ui_BrewScreen_dials); },
                          &isTemperatureStable, &heatingFlash);
    effect_mgr.use_effect([=] { return currentScreen == ui_SimpleProcessScreen; },
                          [=]() { adjustHeatingIndicator(ui_SimpleProcessScreen_dials); }, &isTemperatureStable, &heatingFlash);
    effect_mgr.use_effect([=] { return currentScreen == ui_MenuScreen; }, [=]() { adjustHeatingIndicator(ui_MenuScreen_dials); },
                          &isTemperatureStable, &heatingFlash);
    effect_mgr.use_effect([=] { return currentScreen == ui_ProfileScreen; },
                          [=]() { adjustHeatingIndicator(ui_ProfileScreen_dials); }, &isTemperatureStable, &heatingFlash);
    effect_mgr.use_effect([=] { return currentScreen == ui_GrindScreen; },
                          [=]() { adjustHeatingIndicator(ui_GrindScreen_dials); }, &isTemperatureStable, &heatingFlash);
    effect_mgr.use_effect([=] { return currentScreen == ui_StatusScreen; },
                          [=]() { adjustHeatingIndicator(ui_StatusScreen_dials); }, &isTemperatureStable, &heatingFlash);
    effect_mgr.use_effect([=] { return currentScreen == ui_SimpleProcessScreen; },
                          [=]() { lv_label_set_text(ui_SimpleProcessScreen_mainLabel5, mode == MODE_STEAM ? "Steam" : "Water"); },
                          &mode);
    effect_mgr.use_effect([=] { return currentScreen == ui_MenuScreen; },
                          [=]() {
                              lv_arc_set_value(uic_MenuScreen_dials_tempGauge, currentTemp);
                              lv_label_set_text_fmt(uic_MenuScreen_dials_tempText, "%d°C", currentTemp);
                          },
                          &currentTemp);
    effect_mgr.use_effect([=] { return currentScreen == ui_StatusScreen; },
                          [=]() {
                              lv_arc_set_value(uic_StatusScreen_dials_tempGauge, currentTemp);
                              lv_label_set_text_fmt(uic_StatusScreen_dials_tempText, "%d°C", currentTemp);
                          },
                          &currentTemp);
    effect_mgr.use_effect([=] { return currentScreen == ui_BrewScreen; },
                          [=]() {
                              lv_arc_set_value(uic_BrewScreen_dials_tempGauge, currentTemp);
                              lv_label_set_text_fmt(uic_BrewScreen_dials_tempText, "%d°C", currentTemp);
                          },
                          &currentTemp);
    effect_mgr.use_effect([=] { return currentScreen == ui_GrindScreen; },
                          [=]() {
                              lv_arc_set_value(uic_GrindScreen_dials_tempGauge, currentTemp);
                              lv_label_set_text_fmt(uic_GrindScreen_dials_tempText, "%d°C", currentTemp);
                          },
                          &currentTemp);
    effect_mgr.use_effect([=] { return currentScreen == ui_SimpleProcessScreen; },
                          [=]() {
                              lv_arc_set_value(uic_SimpleProcessScreen_dials_tempGauge, currentTemp);
                              lv_label_set_text_fmt(uic_SimpleProcessScreen_dials_tempText, "%d°C", currentTemp);
                          },
                          &currentTemp);
    effect_mgr.use_effect([=] { return currentScreen == ui_ProfileScreen; },
                          [=]() {
                              lv_arc_set_value(uic_ProfileScreen_dials_tempGauge, currentTemp);
                              lv_label_set_text_fmt(uic_ProfileScreen_dials_tempText, "%d°C", currentTemp);
                          },
                          &currentTemp);
    effect_mgr.use_effect([=] { return currentScreen == ui_MenuScreen; }, [=]() { adjustTempTarget(ui_MenuScreen_dials); },
                          &targetTemp);
    effect_mgr.use_effect([=] { return currentScreen == ui_StatusScreen; },
                          [=]() {
                              lv_label_set_text_fmt(ui_StatusScreen_targetTemp, "%d°C", targetTemp);
                              adjustTempTarget(ui_StatusScreen_dials);
                          },
                          &targetTemp);
    effect_mgr.use_effect([=] { return currentScreen == ui_BrewScreen; },
                          [=]() {
                              lv_label_set_text_fmt(ui_BrewScreen_targetTemp, "%d°C", targetTemp);
                              adjustTempTarget(ui_BrewScreen_dials);
                          },
                          &targetTemp);
    effect_mgr.use_effect([=] { return currentScreen == ui_GrindScreen; }, [=]() { adjustTempTarget(ui_GrindScreen_dials); },
                          &targetTemp);
    effect_mgr.use_effect([=] { return currentScreen == ui_SimpleProcessScreen; },
                          [=]() {
                              lv_label_set_text_fmt(ui_SimpleProcessScreen_targetTemp, "%d°C", targetTemp);
                              adjustTempTarget(ui_SimpleProcessScreen_dials);
                          },
                          &targetTemp);
    effect_mgr.use_effect([=] { return currentScreen == ui_ProfileScreen; }, [=]() { adjustTempTarget(ui_ProfileScreen_dials); },
                          &targetTemp);
    effect_mgr.use_effect([=] { return currentScreen == ui_MenuScreen; },
                          [=]() {
                              lv_arc_set_value(uic_MenuScreen_dials_pressureGauge, pressure * 10.0f);
                              lv_label_set_text_fmt(uic_MenuScreen_dials_pressureText, "%.1f bar", pressure);
                          },
                          &pressure);
    effect_mgr.use_effect([=] { return currentScreen == ui_StatusScreen; },
                          [=]() {
                              lv_arc_set_value(uic_StatusScreen_dials_pressureGauge, pressure * 10.0f);
                              lv_label_set_text_fmt(uic_StatusScreen_dials_pressureText, "%.1f bar", pressure);
                          },
                          &pressure);
    effect_mgr.use_effect([=] { return currentScreen == ui_BrewScreen; },
                          [=]() {
                              lv_arc_set_value(uic_BrewScreen_dials_pressureGauge, pressure * 10.0f);
                              lv_label_set_text_fmt(uic_BrewScreen_dials_pressureText, "%.1f bar", pressure);
                          },
                          &pressure);
    effect_mgr.use_effect([=] { return currentScreen == ui_GrindScreen; },
                          [=]() {
                              lv_arc_set_value(uic_GrindScreen_dials_pressureGauge, pressure * 10.0f);
                              lv_label_set_text_fmt(uic_GrindScreen_dials_pressureText, "%.1f bar", pressure);
                          },
                          &pressure);
    effect_mgr.use_effect([=] { return currentScreen == ui_SimpleProcessScreen; },
                          [=]() {
                              lv_arc_set_value(uic_SimpleProcessScreen_dials_pressureGauge, pressure * 10.0f);
                              lv_label_set_text_fmt(uic_SimpleProcessScreen_dials_pressureText, "%.1f bar", pressure);
                          },
                          &pressure);
    effect_mgr.use_effect([=] { return currentScreen == ui_ProfileScreen; },
                          [=]() {
                              lv_arc_set_value(uic_ProfileScreen_dials_pressureGauge, pressure * 10.0f);
                              lv_label_set_text_fmt(uic_ProfileScreen_dials_pressureText, "%.1f bar", pressure);
                          },
                          &pressure);
    effect_mgr.use_effect([=] { return currentScreen == ui_StandbyScreen; },
                          [=]() {
                              updateAvailable ? lv_obj_clear_flag(ui_StandbyScreen_updateIcon, LV_OBJ_FLAG_HIDDEN)
                                              : lv_obj_add_flag(ui_StandbyScreen_updateIcon, LV_OBJ_FLAG_HIDDEN);
                          },
                          &updateAvailable);
    effect_mgr.use_effect([=] { return currentScreen == ui_InitScreen; },
                          [=]() {
                              if (updateActive) {
                                  lv_label_set_text_fmt(ui_InitScreen_mainLabel, "Updating...");
                              } else if (error) {
                                  if (controller->getError() == ERROR_CODE_RUNAWAY) {
                                      lv_label_set_text_fmt(ui_InitScreen_mainLabel, "Temperature error, please restart");
                                  }
                              } else if (autotuning) {
                                  lv_label_set_text_fmt(ui_InitScreen_mainLabel, "Autotuning...");
                              } else if (waitingForController) {
                                  lv_label_set_text_fmt(ui_InitScreen_mainLabel, "Waiting for controller...");
                              }
                          },
                          &updateAvailable, &error, &autotuning, &waitingForController);
    effect_mgr.use_effect([=] { return currentScreen == ui_BrewScreen; },
                          [=]() {
                              if (volumetricMode) {
                                  lv_label_set_text_fmt(ui_BrewScreen_targetDuration, "%.1fg", targetVolume);
                              } else {
                                  const double secondsDouble = targetDuration;
                                  const auto minutes = static_cast<int>(secondsDouble / 60.0);
                                  const auto seconds = static_cast<int>(secondsDouble) % 60;
                                  lv_label_set_text_fmt(ui_BrewScreen_targetDuration, "%2d:%02d", minutes, seconds);
                              }
                          },
                          &targetDuration, &targetVolume, &volumetricMode);
    effect_mgr.use_effect([=] { return currentScreen == ui_GrindScreen; },
                          [=]() {
                              if (volumetricMode) {
                                  lv_label_set_text_fmt(ui_GrindScreen_targetDuration, "%.1fg", grindVolume);
                              } else {
                                  const double secondsDouble = grindDuration / 1000.0;
                                  const auto minutes = static_cast<int>(secondsDouble / 60.0);
                                  const auto seconds = static_cast<int>(secondsDouble) % 60;
                                  lv_label_set_text_fmt(ui_GrindScreen_targetDuration, "%2d:%02d", minutes, seconds);
                              }
                          },
                          &grindDuration, &grindVolume, &volumetricMode);
    effect_mgr.use_effect(
        [=] { return currentScreen == ui_BrewScreen; },
        [=]() {
            lv_img_set_src(ui_BrewScreen_Image4, volumetricMode ? &ui_img_1424216268 : &ui_img_360122106);
            ui_object_set_themeable_style_property(ui_BrewScreen_weightLabel, LV_PART_MAIN | LV_STATE_DEFAULT,
                                                   LV_STYLE_TEXT_COLOR,
                                                   volumetricMode ? _ui_theme_color_Dark : _ui_theme_color_NiceWhite);
            ui_object_set_themeable_style_property(ui_BrewScreen_volumetricButton, LV_PART_MAIN | LV_STATE_DEFAULT,
                                                   LV_STYLE_IMG_RECOLOR,
                                                   volumetricMode ? _ui_theme_color_Dark : _ui_theme_color_NiceWhite);
            ui_object_set_themeable_style_property(ui_BrewScreen_modeSwitch, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_BG_COLOR,
                                                   volumetricMode ? _ui_theme_color_NiceWhite : _ui_theme_color_Dark);
        },
        &volumetricMode);
    effect_mgr.use_effect(
        [=] { return currentScreen == ui_GrindScreen; },
        [=]() {
            lv_img_set_src(ui_GrindScreen_targetSymbol, volumetricMode ? &ui_img_1424216268 : &ui_img_360122106);
            ui_object_set_themeable_style_property(ui_GrindScreen_weightLabel, LV_PART_MAIN | LV_STATE_DEFAULT,
                                                   LV_STYLE_TEXT_COLOR,
                                                   volumetricMode ? _ui_theme_color_Dark : _ui_theme_color_NiceWhite);
            ui_object_set_themeable_style_property(ui_GrindScreen_volumetricButton, LV_PART_MAIN | LV_STATE_DEFAULT,
                                                   LV_STYLE_IMG_RECOLOR,
                                                   volumetricMode ? _ui_theme_color_Dark : _ui_theme_color_NiceWhite);
            ui_object_set_themeable_style_property(ui_GrindScreen_modeSwitch, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_BG_COLOR,
                                                   volumetricMode ? _ui_theme_color_NiceWhite : _ui_theme_color_Dark);
        },
        &volumetricMode);
    effect_mgr.use_effect([=] { return currentScreen == ui_GrindScreen; },
                          [=]() { _ui_flag_modify(ui_GrindScreen_modeSwitch, LV_OBJ_FLAG_HIDDEN, volumetricAvailable); },
                          &volumetricAvailable);
    effect_mgr.use_effect([=] { return currentScreen == ui_SimpleProcessScreen; },
                          [=]() {
                              if (mode == MODE_STEAM) {
                                  _ui_flag_modify(ui_SimpleProcessScreen_goButton, LV_OBJ_FLAG_HIDDEN, active);
                                  lv_imgbtn_set_src(ui_SimpleProcessScreen_goButton, LV_IMGBTN_STATE_RELEASED, nullptr,
                                                    &ui_img_691326438, nullptr);
                              } else {
                                  lv_imgbtn_set_src(ui_SimpleProcessScreen_goButton, LV_IMGBTN_STATE_RELEASED, nullptr,
                                                    active ? &ui_img_1456692430 : &ui_img_445946954, nullptr);
                              }
                          },
                          &active, &mode);
    effect_mgr.use_effect([=] { return currentScreen == ui_GrindScreen; },
                          [=]() {
                              lv_imgbtn_set_src(ui_GrindScreen_startButton, LV_IMGBTN_STATE_RELEASED, nullptr,
                                                grindActive ? &ui_img_1456692430 : &ui_img_445946954, nullptr);
                          },
                          &grindActive);
    effect_mgr.use_effect([=] { return currentScreen == ui_BrewScreen; },
                          [=] { lv_label_set_text(ui_BrewScreen_profileName, selectedProfile.label.c_str()); },
                          &selectedProfileId);

    effect_mgr.use_effect(
        [=] { return currentScreen == ui_ProfileScreen; },
        [=] {
            if (profileLoaded) {
                _ui_flag_modify(ui_ProfileScreen_profileDetails, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
                _ui_flag_modify(ui_ProfileScreen_loadingSpinner, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                lv_label_set_text(ui_ProfileScreen_profileName, currentProfileChoice.label.c_str());

                const auto minutes = static_cast<int>(currentProfileChoice.getTotalDuration() / 60.0 - 0.5);
                const auto seconds = static_cast<int>(currentProfileChoice.getTotalDuration()) % 60;
                lv_label_set_text_fmt(ui_ProfileScreen_targetDuration2, "%2d:%02d", minutes, seconds);
                lv_label_set_text_fmt(ui_ProfileScreen_targetTemp2, "%d°C", static_cast<int>(currentProfileChoice.temperature));
                unsigned int phaseCount = currentProfileChoice.getPhaseCount();
                unsigned int stepCount = currentProfileChoice.phases.size();
                lv_label_set_text_fmt(ui_ProfileScreen_stepsLabel, "%d step%s", stepCount, stepCount > 1 ? "s" : "");
                lv_label_set_text_fmt(ui_ProfileScreen_phasesLabel, "%d phase%s", phaseCount, phaseCount > 1 ? "s" : "");
            } else {
                _ui_flag_modify(ui_ProfileScreen_profileDetails, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                _ui_flag_modify(ui_ProfileScreen_loadingSpinner, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            }

            ui_object_set_themeable_style_property(ui_ProfileScreen_previousProfileBtn, LV_PART_MAIN | LV_STATE_DEFAULT,
                                                   LV_STYLE_IMG_RECOLOR,
                                                   currentProfileIdx > 0 ? _ui_theme_color_NiceWhite : _ui_theme_color_SemiDark);
            ui_object_set_themeable_style_property(ui_ProfileScreen_previousProfileBtn, LV_PART_MAIN | LV_STATE_DEFAULT,
                                                   LV_STYLE_IMG_RECOLOR_OPA,
                                                   currentProfileIdx > 0 ? _ui_theme_alpha_NiceWhite : _ui_theme_alpha_SemiDark);
            ui_object_set_themeable_style_property(
                ui_ProfileScreen_nextProfileBtn, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_IMG_RECOLOR,
                currentProfileIdx < favoritedProfiles.size() - 1 ? _ui_theme_color_NiceWhite : _ui_theme_color_SemiDark);
            ui_object_set_themeable_style_property(
                ui_ProfileScreen_nextProfileBtn, LV_PART_MAIN | LV_STATE_DEFAULT, LV_STYLE_IMG_RECOLOR_OPA,
                currentProfileIdx < favoritedProfiles.size() - 1 ? _ui_theme_alpha_NiceWhite : _ui_theme_alpha_SemiDark);
        },
        &currentProfileId, &profileLoaded);

    // Show/hide grind button based on SmartGrind setting or Alt Relay function
    effect_mgr.use_effect([=] { return currentScreen == ui_MenuScreen; },
                          [=]() {
                              grindAvailable ? lv_obj_clear_flag(ui_MenuScreen_grindBtn, LV_OBJ_FLAG_HIDDEN)
                                             : lv_obj_add_flag(ui_MenuScreen_grindBtn, LV_OBJ_FLAG_HIDDEN);
                          },
                          &grindAvailable);
    effect_mgr.use_effect([=] { return currentScreen == ui_BrewScreen; },
                          [=]() {
                              if (volumetricAvailable && bluetoothScales) {
                                  lv_label_set_text_fmt(ui_BrewScreen_weightLabel, "%.1fg", bluetoothWeight);
                              } else {
                                  lv_label_set_text(ui_BrewScreen_weightLabel, "-");
                              }
                          },
                          &bluetoothWeight, &volumetricAvailable, &bluetoothScales);
    effect_mgr.use_effect([=] { return currentScreen == ui_GrindScreen; },
                          [=]() {
                              if (volumetricAvailable && bluetoothScales) {
                                  lv_label_set_text_fmt(ui_GrindScreen_weightLabel, "%.1fg", bluetoothWeight);
                              } else {
                                  lv_label_set_text(ui_GrindScreen_weightLabel, "-");
                              }
                          },
                          &bluetoothWeight, &volumetricAvailable, &bluetoothScales);
    effect_mgr.use_effect(
        [=] { return currentScreen == ui_BrewScreen; },
        [=]() {
            _ui_flag_modify(ui_BrewScreen_adjustments, LV_OBJ_FLAG_HIDDEN, brewScreenState == BrewScreenState::Settings);
            _ui_flag_modify(ui_BrewScreen_acceptButton, LV_OBJ_FLAG_HIDDEN, brewScreenState == BrewScreenState::Settings);
            _ui_flag_modify(ui_BrewScreen_saveButton, LV_OBJ_FLAG_HIDDEN, brewScreenState == BrewScreenState::Settings);
            _ui_flag_modify(ui_BrewScreen_saveAsNewButton, LV_OBJ_FLAG_HIDDEN, brewScreenState == BrewScreenState::Settings);
            _ui_flag_modify(ui_BrewScreen_startButton, LV_OBJ_FLAG_HIDDEN, brewScreenState == BrewScreenState::Brew);
            _ui_flag_modify(ui_BrewScreen_profileInfo, LV_OBJ_FLAG_HIDDEN, brewScreenState == BrewScreenState::Brew);
            _ui_flag_modify(ui_BrewScreen_modeSwitch, LV_OBJ_FLAG_HIDDEN,
                            brewScreenState == BrewScreenState::Brew && volumetricAvailable);
            if (volumetricAvailable) {
                lv_img_set_src(ui_BrewScreen_volumetricButton, bluetoothScales ? &ui_img_1424216268 : &ui_img_flowmeter_png);
            }
        },
        &brewScreenState, &volumetricAvailable, &bluetoothScales);
    effect_mgr.use_effect([=] { return currentScreen == ui_StandbyScreen; },
                          [=]() { lv_img_set_src(ui_StandbyScreen_logo, christmasMode ? &ui_img_1510335 : &ui_img_logo_png); },
                          &christmasMode);

    // === New screen reactive effects ===

    // Brew screen temp update
    effect_mgr.use_effect(
        [=] { return currentScreen == ui_NewBrewScreen && ui_NewBrewScreen_tempLabel != NULL; },
        [=]() {
            char buf[16];
            snprintf(buf, sizeof(buf), "%d\xC2\xB0", currentTemp);
            lv_label_set_text(ui_NewBrewScreen_tempLabel, buf);
            lv_arc_set_value(ui_NewBrewScreen_tempArc, currentTemp);
        },
        &currentTemp);

    // Brew screen pressure update
    effect_mgr.use_effect(
        [=] { return currentScreen == ui_NewBrewScreen && pressureAvailable && ui_NewBrewScreen_pressureLabel != NULL; },
        [=]() {
            char buf[16];
            snprintf(buf, sizeof(buf), "%.1f bar", pressure);
            lv_label_set_text(ui_NewBrewScreen_pressureLabel, buf);
            lv_arc_set_value(ui_NewBrewScreen_pressureArc, (int)(pressure * 10));
        },
        &pressure);

    // Water screen temp update
    effect_mgr.use_effect(
        [=] { return currentScreen == ui_NewWaterScreen && ui_NewWaterScreen_tempLabel != NULL; },
        [=]() {
            char buf[16];
            snprintf(buf, sizeof(buf), "%d\xC2\xB0", currentTemp);
            lv_label_set_text(ui_NewWaterScreen_tempLabel, buf);
            lv_arc_set_value(ui_NewWaterScreen_tempArc, currentTemp);
            int target = controller->getTargetTemp();
            if (currentTemp >= target) {
                lv_label_set_text(ui_NewWaterScreen_statusLabel, "ready");
                lv_obj_set_style_text_color(ui_NewWaterScreen_statusLabel, UI_COLOR_GREEN, LV_PART_MAIN);
            } else {
                lv_label_set_text(ui_NewWaterScreen_statusLabel, "heating...");
                lv_obj_set_style_text_color(ui_NewWaterScreen_statusLabel, UI_COLOR_BLUE, LV_PART_MAIN);
            }
        },
        &currentTemp);

    // Steam screen temp update
    effect_mgr.use_effect(
        [=] { return currentScreen == ui_NewSteamScreen && ui_NewSteamScreen_tempLabel != NULL; },
        [=]() {
            char buf[16];
            snprintf(buf, sizeof(buf), "%d\xC2\xB0", currentTemp);
            lv_label_set_text(ui_NewSteamScreen_tempLabel, buf);
            lv_arc_set_value(ui_NewSteamScreen_tempArc, currentTemp);
            int target = controller->getTargetTemp();
            bool ready = currentTemp >= target;
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
        &currentTemp);

    // Unified screen temp + target update
    effect_mgr.use_effect(
        [=] { return currentScreen == ui_UnifiedScreen && ui_UnifiedScreen_tempLabel != NULL; },
        [=]() {
            char buf[16];
            // Update current/target temp
            int target = controller->getTargetTemp();
            if (target > 0) {
                snprintf(buf, sizeof(buf), "%d/%d\xC2\xB0", currentTemp, target);
            } else {
                snprintf(buf, sizeof(buf), "%d\xC2\xB0", currentTemp);
            }
            lv_label_set_text(ui_UnifiedScreen_tempLabel, buf);
            // Update the appropriate arc based on mode
            if (!lv_obj_has_flag(ui_UnifiedScreen_innerArc, LV_OBJ_FLAG_HIDDEN)) {
                // Brew mode: outer arc is temp (red/amber)
                lv_arc_set_value(ui_UnifiedScreen_outerArc, currentTemp);
            } else {
                // Water/Steam mode: outer arc is temp
                lv_arc_set_value(ui_UnifiedScreen_outerArc, currentTemp);
                // Steam: change ring color when ready
                if (mode == MODE_STEAM) {
                    int target = controller->getTargetTemp();
                    bool ready = currentTemp >= target;
                    lv_color_t ringColor = ready ? UI_COLOR_GREEN : UI_COLOR_AMBER;
                    lv_obj_set_style_arc_color(ui_UnifiedScreen_outerArc, ringColor, LV_PART_INDICATOR);
                }
            }
        },
        &currentTemp);

    // Unified screen pressure update
    effect_mgr.use_effect(
        [=] { return currentScreen == ui_UnifiedScreen && pressureAvailable && ui_UnifiedScreen_pressureLabel != NULL; },
        [=]() {
            char buf[16];
            snprintf(buf, sizeof(buf), "%.1f", pressure);
            lv_label_set_text(ui_UnifiedScreen_pressureLabel, buf);
            // Only update inner arc with pressure in brew mode (when inner arc is visible)
            if (!lv_obj_has_flag(ui_UnifiedScreen_innerArc, LV_OBJ_FLAG_HIDDEN)) {
                lv_arc_set_value(ui_UnifiedScreen_innerArc, (int)(pressure * 10));
            }
        },
        &pressure);

    // Standby WiFi/BT icon states
    effect_mgr.use_effect(
        [=] { return currentScreen == ui_NewStandbyScreen && ui_NewStandbyScreen_wifiIcon != NULL; },
        [=]() {
            bool wifiConnected = WiFi.status() == WL_CONNECTED;
            lv_color_t wifiColor = (wifiConnected || apActive) ? UI_COLOR_STANDBY_ICON_PRI : UI_COLOR_ICON_DISCONNECTED;
            lv_obj_set_style_text_color(ui_NewStandbyScreen_wifiIcon, wifiColor, LV_PART_MAIN);
            bool btConnected = !waitingForController;
            lv_color_t btColor = btConnected ? UI_COLOR_STANDBY_ICON_SEC : UI_COLOR_ICON_DISCONNECTED;
            lv_obj_set_style_text_color(ui_NewStandbyScreen_btIcon, btColor, LV_PART_MAIN);
            lv_obj_set_style_text_opa(ui_NewStandbyScreen_btIcon,
                btConnected ? UI_STANDBY_ICON_SEC_OPA : LV_OPA_COVER, LV_PART_MAIN);
        },
        &waitingForController);

    // Unified screen standby WiFi/BT icon states
    effect_mgr.use_effect(
        [=] { return currentScreen == ui_UnifiedScreen && ui_UnifiedScreen_wifiLabel != NULL; },
        [=]() {
            bool wifiConnected = WiFi.status() == WL_CONNECTED;
            lv_color_t wifiColor = (wifiConnected || apActive) ? UI_COLOR_STANDBY_ICON_PRI : UI_COLOR_ICON_DISCONNECTED;
            lv_obj_set_style_text_color(ui_UnifiedScreen_wifiLabel, wifiColor, LV_PART_MAIN);
            bool btConnected = !waitingForController;
            lv_color_t btColor = btConnected ? UI_COLOR_STANDBY_ICON_SEC : UI_COLOR_ICON_DISCONNECTED;
            lv_obj_set_style_text_color(ui_UnifiedScreen_btLabel, btColor, LV_PART_MAIN);
            lv_obj_set_style_text_opa(ui_UnifiedScreen_btLabel,
                btConnected ? UI_STANDBY_ICON_SEC_OPA : LV_OPA_COVER, LV_PART_MAIN);
        },
        &waitingForController);
}

void DefaultUI::handleScreenChange() {
    lv_obj_t *current = lv_scr_act();

    if (current != *targetScreen) {
        if (*targetScreen == ui_StandbyScreen || *targetScreen == ui_NewStandbyScreen) {
            standbyEnterTime = millis();
        } else if (current == ui_StandbyScreen || current == ui_NewStandbyScreen) {
            const Settings &settings = controller->getSettings();
            setBrightness(settings.getMainBrightness());
        }

        _ui_screen_change(targetScreen, LV_SCR_LOAD_ANIM_NONE, 0, 0, targetScreenInit);
        lv_obj_del(current);

        // After unified screen init, apply the correct mode
        if (*targetScreen == ui_UnifiedScreen && ui_UnifiedScreen_tempLabel != NULL) {
            switch (mode) {
            case MODE_STANDBY:
                ui_UnifiedScreen_set_mode_standby();
                break;
            case MODE_WATER:
                ui_UnifiedScreen_set_mode_water();
                break;
            case MODE_STEAM:
                ui_UnifiedScreen_set_mode_steam();
                break;
            default:
                ui_UnifiedScreen_set_mode_brew();
                break;
            }
        }

        rerender = true;
    }
}

void DefaultUI::updateStandbyScreen() {
    if (standbyEnterTime > 0) {
        const Settings &settings = controller->getSettings();
        const unsigned long now = millis();
        if (now - standbyEnterTime >= settings.getStandbyBrightnessTimeout()) {
            setBrightness(settings.getStandbyBrightness());
        }
    }

    if (!apActive && WiFi.status() == WL_CONNECTED) {
        time_t now;
        struct tm timeinfo;

        localtime_r(&now, &timeinfo);
        // allocate enough space for both 12h/24h time formats
        if (getLocalTime(&timeinfo, 500)) {
            char time[9];
            Settings &settings = controller->getSettings();
            const char *format = settings.isClock24hFormat() ? "%H:%M" : "%I:%M %p";
            strftime(time, sizeof(time), format, &timeinfo);
            lv_label_set_text(ui_StandbyScreen_time, time);
            lv_obj_clear_flag(ui_StandbyScreen_time, LV_OBJ_FLAG_HIDDEN);

            christmasMode = (timeinfo.tm_mon == 11 && timeinfo.tm_mday < 27) || (timeinfo.tm_mon == 0 && timeinfo.tm_mday < 6);
        }
    } else {
        lv_obj_add_flag(ui_StandbyScreen_time, LV_OBJ_FLAG_HIDDEN);
    }
    controller->getClientController()->isConnected() ? lv_obj_clear_flag(ui_StandbyScreen_bluetoothIcon, LV_OBJ_FLAG_HIDDEN)
                                                     : lv_obj_add_flag(ui_StandbyScreen_bluetoothIcon, LV_OBJ_FLAG_HIDDEN);
    !apActive &&WiFi.status() == WL_CONNECTED ? lv_obj_clear_flag(ui_StandbyScreen_wifiIcon, LV_OBJ_FLAG_HIDDEN)
                                              : lv_obj_add_flag(ui_StandbyScreen_wifiIcon, LV_OBJ_FLAG_HIDDEN);
}

void DefaultUI::updateStatusScreen() const {
    // Copy process pointers to avoid race conditions with controller thread
    Process *process = controller->getProcess();
    Process *lastProcess = controller->getLastProcess();

    if (process == nullptr) {
        process = lastProcess;
    }
    if (process == nullptr || process->getType() != MODE_BREW) {
        return;
    }

    // Additional safety: Validate that the process pointer is still valid
    // by checking if it matches either current or last process
    if (process != controller->getProcess() && process != controller->getLastProcess()) {
        ESP_LOGW("DefaultUI", "Process pointer became invalid during access, skipping update");
        return;
    }

    auto *brewProcess = static_cast<BrewProcess *>(process);
    if (brewProcess == nullptr) {
        ESP_LOGE("DefaultUI", "brewProcess is null after cast");
        return;
    }

    // Validate the brewProcess object before accessing its members
    // Check if the object is in a reasonable state by validating key fields
    if (brewProcess->profile.phases.empty() || brewProcess->phaseIndex >= brewProcess->profile.phases.size()) {
        ESP_LOGE("DefaultUI", "brewProcess phaseIndex out of bounds: %u >= %zu", brewProcess->phaseIndex,
                 brewProcess->profile.phases.size());
        return;
    }

    // Final safety check before accessing brewProcess members
    if (!brewProcess) {
        ESP_LOGE("DefaultUI", "brewProcess became null after validation");
        return;
    }

    const auto phase = brewProcess->currentPhase;

    unsigned long now = millis();
    if (!process->isActive()) {
        // Add bounds check for finished timestamp
        if (brewProcess && brewProcess->finished > 0) {
            now = brewProcess->finished;
        }
    }

    lv_label_set_text(ui_StatusScreen_stepLabel, phase.phase == PhaseType::PHASE_TYPE_BREW ? "BREW" : "INFUSION");
    lv_label_set_text(ui_StatusScreen_phaseLabel, brewProcess && brewProcess->isActive() ? phase.name.c_str() : "Finished");

    // Add bounds check for processStarted timestamp
    if (brewProcess && brewProcess->processStarted > 0 && now >= brewProcess->processStarted) {
        const unsigned long processDuration = now - brewProcess->processStarted;
        const double processSecondsDouble = processDuration / 1000.0;
        const auto processMinutes = static_cast<int>(processSecondsDouble / 60.0);
        const auto processSeconds = static_cast<int>(processSecondsDouble) % 60;
        lv_label_set_text_fmt(ui_StatusScreen_currentDuration, "%2d:%02d", processMinutes, processSeconds);
    } else {
        lv_label_set_text_fmt(ui_StatusScreen_currentDuration, "00:00");
    }

    if (brewProcess && brewProcess->target == ProcessTarget::VOLUMETRIC && phase.hasVolumetricTarget()) {
        Target target = phase.getVolumetricTarget();
        lv_bar_set_value(ui_StatusScreen_brewBar, brewProcess->currentVolume * 10.0, LV_ANIM_OFF);
        lv_bar_set_range(ui_StatusScreen_brewBar, 0, target.value * 10.0 + 1.0);
        lv_label_set_text_fmt(ui_StatusScreen_brewLabel, "%.1fg", target.value);
    } else if (brewProcess) {
        // Add bounds check for currentPhaseStarted timestamp
        if (brewProcess->currentPhaseStarted > 0 && now >= brewProcess->currentPhaseStarted) {
            const unsigned long progress = now - brewProcess->currentPhaseStarted;
            lv_bar_set_value(ui_StatusScreen_brewBar, progress, LV_ANIM_OFF);
            lv_bar_set_range(ui_StatusScreen_brewBar, 0, std::max(static_cast<int>(brewProcess->getPhaseDuration()), 1));
            lv_label_set_text_fmt(ui_StatusScreen_brewLabel, "%ds", brewProcess->getPhaseDuration() / 1000);
        } else {
            lv_bar_set_value(ui_StatusScreen_brewBar, 0, LV_ANIM_OFF);
            lv_bar_set_range(ui_StatusScreen_brewBar, 0, 1);
            lv_label_set_text(ui_StatusScreen_brewLabel, "0s");
        }
    }

    if (brewProcess && brewProcess->target == ProcessTarget::TIME) {
        const unsigned long targetDuration = brewProcess->getTotalDuration();
        const double targetSecondsDouble = targetDuration / 1000.0;
        const auto targetMinutes = static_cast<int>(targetSecondsDouble / 60.0);
        const auto targetSeconds = static_cast<int>(targetSecondsDouble) % 60;
        lv_label_set_text_fmt(ui_StatusScreen_targetDuration, "%2d:%02d", targetMinutes, targetSeconds);
    } else if (brewProcess) {
        lv_label_set_text_fmt(ui_StatusScreen_targetDuration, "%.1fg", brewProcess->getBrewVolume());
    }
    if (brewProcess) {
        lv_img_set_src(ui_StatusScreen_Image8,
                       brewProcess->target == ProcessTarget::TIME ? &ui_img_360122106 : &ui_img_1424216268);
    }

    if (brewProcess && brewProcess->isAdvancedPump()) {
        float pressure = brewProcess->getPumpPressure();
        const double percentage = 1.0 - static_cast<double>(pressure) / static_cast<double>(pressureScaling);
        adjustTarget(uic_StatusScreen_dials_pressureTarget, percentage, -62.0, 124.0);
    } else {
        const double percentage = 1.0 - 0.5;
        adjustTarget(uic_StatusScreen_dials_pressureTarget, percentage, -62.0, 124.0);
    }

    // Brew finished adjustments
    if (process->isActive()) {
        lv_obj_add_flag(ui_StatusScreen_brewVolume, LV_OBJ_FLAG_HIDDEN);
    } else {
        // Re-validate brewProcess pointer before accessing members
        if (brewProcess && brewProcess->target == ProcessTarget::VOLUMETRIC) {
            lv_obj_clear_flag(ui_StatusScreen_brewVolume, LV_OBJ_FLAG_HIDDEN);
        }
        lv_obj_add_flag(ui_StatusScreen_barContainer, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_StatusScreen_labelContainer, LV_OBJ_FLAG_HIDDEN);
        if (brewProcess) {
            lv_label_set_text_fmt(ui_StatusScreen_brewVolume, "%.1lfg", brewProcess->currentVolume);
        }
        lv_imgbtn_set_src(ui_StatusScreen_pauseButton, LV_IMGBTN_STATE_RELEASED, nullptr, &ui_img_631115820, nullptr);
    }
}

void DefaultUI::updateUnifiedScreen() {
    if (!isBrewing || currentScreen != ui_UnifiedScreen || ui_UnifiedScreen_timerLabel == NULL) return;

    Process *process = controller->getProcess();
    if (process == nullptr || process->getType() != MODE_BREW) {
        return;
    }

    auto *brewProcess = static_cast<BrewProcess *>(process);
    if (brewProcess == nullptr) return;

    unsigned long elapsed = (millis() - brewProcess->processStarted) / 1000;
    char buf[8];
    snprintf(buf, sizeof(buf), "%lu:%02lu", elapsed / 60, elapsed % 60);
    lv_label_set_text(ui_UnifiedScreen_timerLabel, buf);

    if (!brewProcess->currentPhase.name.isEmpty()) {
        lv_label_set_text(ui_UnifiedScreen_phaseLabel, brewProcess->currentPhase.name.c_str());
    }

    if (brewProcess->isComplete() && !isBrewComplete) {
        isBrewComplete = true;
        brewCompleteTime = millis();
        ui_UnifiedScreen_set_complete();
    }

    // Auto-dismiss complete after 5 seconds
    if (isBrewComplete && millis() - brewCompleteTime > UI_BREW_COMPLETE_DISMISS_MS) {
        isBrewComplete = false;
        isBrewing = false;
        ui_UnifiedScreen_set_idle();
    }
}

void DefaultUI::updateNewBrewScreen() {
    if (!isBrewing || currentScreen != ui_NewBrewScreen || ui_NewBrewScreen_timerLabel == NULL) return;

    Process *process = controller->getProcess();
    if (process == nullptr || process->getType() != MODE_BREW) {
        return;
    }

    auto *brewProcess = static_cast<BrewProcess *>(process);
    if (brewProcess == nullptr) return;

    unsigned long elapsed = (millis() - brewProcess->processStarted) / 1000;
    char buf[8];
    snprintf(buf, sizeof(buf), "%lu:%02lu", elapsed / 60, elapsed % 60);
    lv_label_set_text(ui_NewBrewScreen_timerLabel, buf);

    if (!brewProcess->currentPhase.name.isEmpty()) {
        lv_label_set_text(ui_NewBrewScreen_phaseLabel, brewProcess->currentPhase.name.c_str());
    }

    if (brewProcess->isComplete() && !isBrewComplete) {
        isBrewComplete = true;
        brewCompleteTime = millis();
        ui_NewBrewScreen_set_complete();
    }

    // Auto-dismiss complete after 5 seconds
    if (isBrewComplete && millis() - brewCompleteTime > UI_BREW_COMPLETE_DISMISS_MS) {
        isBrewComplete = false;
        isBrewing = false;
        ui_NewBrewScreen_set_idle();
    }
}

void DefaultUI::updateNewWaterScreen() {
    // Placeholder for future water screen updates
}

void DefaultUI::updateNewSteamScreen() {
    // Placeholder for future steam screen updates
}

void DefaultUI::updateNewStandbyScreen() {
    // Handle standby brightness dimming for new standby screen
    if (currentScreen == ui_NewStandbyScreen && standbyEnterTime > 0) {
        const Settings &settings = controller->getSettings();
        const unsigned long now = millis();
        if (now - standbyEnterTime >= settings.getStandbyBrightnessTimeout()) {
            setBrightness(settings.getStandbyBrightness());
        }
    }
}

void DefaultUI::adjustDials(lv_obj_t *dials) {
    lv_obj_t *tempGauge = ui_comp_get_child(dials, UI_COMP_DIALS_TEMPGAUGE);
    lv_obj_t *tempText = ui_comp_get_child(dials, UI_COMP_DIALS_TEMPTEXT);
    lv_obj_t *pressureTarget = ui_comp_get_child(dials, UI_COMP_DIALS_PRESSURETARGET);
    lv_obj_t *pressureGauge = ui_comp_get_child(dials, UI_COMP_DIALS_PRESSUREGAUGE);
    lv_obj_t *pressureText = ui_comp_get_child(dials, UI_COMP_DIALS_PRESSURETEXT);
    lv_obj_t *pressureSymbol = ui_comp_get_child(dials, UI_COMP_DIALS_IMAGE6);
    _ui_flag_modify(pressureTarget, LV_OBJ_FLAG_HIDDEN, pressureAvailable);
    _ui_flag_modify(pressureGauge, LV_OBJ_FLAG_HIDDEN, pressureAvailable);
    _ui_flag_modify(pressureText, LV_OBJ_FLAG_HIDDEN, pressureAvailable);
    _ui_flag_modify(pressureSymbol, LV_OBJ_FLAG_HIDDEN, pressureAvailable);
    lv_obj_set_x(tempText, pressureAvailable ? -50 : 0);
    lv_obj_set_y(tempText, pressureAvailable ? -205 : -180);
    lv_arc_set_bg_angles(tempGauge, 118, pressureAvailable ? 242 : 62);
    lv_arc_set_range(pressureGauge, 0, pressureScaling * 10);
}

inline void DefaultUI::adjustTempTarget(lv_obj_t *dials) {
    double gaugeAngle = pressureAvailable ? 124.0 : 304;
    double gaugeStart = pressureAvailable ? 118.0 : -62;
    double percentage = static_cast<double>(targetTemp) / 160.0;
    lv_obj_t *tempTarget = ui_comp_get_child(dials, UI_COMP_DIALS_TEMPTARGET);
    adjustTarget(tempTarget, percentage, gaugeStart, gaugeAngle);
}

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
}

void DefaultUI::adjustTarget(lv_obj_t *obj, double percentage, double start, double range) const {
    double angle = start + range - range * percentage;

    lv_img_set_angle(obj, angle * -10);
    int x = static_cast<int>(std::cos(angle * M_PI / 180.0f) * 235.0);
    int y = static_cast<int>(std::sin(angle * M_PI / 180.0f) * -235.0);
    lv_obj_set_pos(obj, x, y);
}

void DefaultUI::loopTask(void *arg) {
    auto *ui = static_cast<DefaultUI *>(arg);
    while (true) {
        ui->loop();
        vTaskDelay(25 / portTICK_PERIOD_MS);
    }
}

void DefaultUI::profileLoopTask(void *arg) {
    auto *ui = static_cast<DefaultUI *>(arg);
    while (true) {
        ui->loopProfiles();
        vTaskDelay(25 / portTICK_PERIOD_MS);
    }
}
