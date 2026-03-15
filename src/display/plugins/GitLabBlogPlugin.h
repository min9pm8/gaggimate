#ifndef GITLABBLOGPLUGIN_H
#define GITLABBLOGPLUGIN_H

#include "../core/Plugin.h"
#include <Arduino.h>

struct GitLabBlogStatus {
    int httpCode = 0;
    String message = "";
    unsigned long timestamp = 0;
};

struct ActivityRecord {
    String type;
    float startTemp = 0;
    float endTemp = 0;
    unsigned long durationMs = 0;
    time_t timestamp = 0;
};

constexpr int MAX_ACTIVITIES = 8;
constexpr int MAX_PENDING_SHOTS = 4;
constexpr unsigned long PUBLISH_DELAY_MS = 5 * 60 * 1000; // 5 min after last event

class GitLabBlogPlugin : public Plugin {
  public:
    void setup(Controller *controller, PluginManager *pluginManager) override;
    void loop() override;

    GitLabBlogStatus testConnection();
    GitLabBlogStatus getLastPublishStatus() const { return lastStatus; }

  private:
    void publishCombinedPost();
    Controller *controller = nullptr;
    PluginManager *pluginManager = nullptr;

    float lastTemperature = 0;
    float lastPressure = 0;
    float lastPumpFlow = 0;
    float lastPuckFlow = 0;
    float lastBluetoothWeight = 0;
    float lastEstimatedWeight = 0;
    unsigned long brewStartTime = 0;

    // Steam/water activity tracking
    int activeMode = 0;
    float processStartTemp = 0;
    unsigned long processStartTime = 0;

    // Deferred publish: accumulate shots + activities into one post
    int pendingShots[MAX_PENDING_SHOTS];
    int pendingShotCount = 0;
    unsigned long lastEventTime = 0;
    ActivityRecord activities[MAX_ACTIVITIES];
    int activityCount = 0;

    GitLabBlogStatus lastStatus;
};

#endif // GITLABBLOGPLUGIN_H
