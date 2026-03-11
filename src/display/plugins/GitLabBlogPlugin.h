#ifndef GITLABBLOGPLUGIN_H
#define GITLABBLOGPLUGIN_H

#include "../core/Plugin.h"
#include <Arduino.h>

struct GitLabBlogStatus {
    int httpCode = 0;
    String message = "";
    unsigned long timestamp = 0;
};

class GitLabBlogPlugin : public Plugin {
  public:
    void setup(Controller *controller, PluginManager *pluginManager) override;
    void loop() override {};

    GitLabBlogStatus testConnection();
    GitLabBlogStatus getLastPublishStatus() const { return lastStatus; }

  private:
    void publishShot(Controller *controller);
    Controller *controller = nullptr;
    PluginManager *pluginManager = nullptr;

    float lastTemperature = 0;
    float lastPressure = 0;
    float lastPumpFlow = 0;
    float lastPuckFlow = 0;
    float lastBluetoothWeight = 0;
    float lastEstimatedWeight = 0;
    unsigned long brewStartTime = 0;

    GitLabBlogStatus lastStatus;
};

#endif // GITLABBLOGPLUGIN_H
