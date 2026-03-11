#ifndef GITLABBLOGPLUGIN_H
#define GITLABBLOGPLUGIN_H

#include "../core/Plugin.h"
#include <Arduino.h>

class GitLabBlogPlugin : public Plugin {
  public:
    void setup(Controller *controller, PluginManager *pluginManager) override;
    void loop() override {};

  private:
    void publishShot(Controller *controller);
    Controller *controller = nullptr;

    float lastTemperature = 0;
    float lastPressure = 0;
    float lastPumpFlow = 0;
    float lastPuckFlow = 0;
    float lastBluetoothWeight = 0;
    float lastEstimatedWeight = 0;
    unsigned long brewStartTime = 0;
};

#endif // GITLABBLOGPLUGIN_H
