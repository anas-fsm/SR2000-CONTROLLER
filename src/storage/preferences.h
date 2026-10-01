#ifndef PREFERENCES_MANAGER_H
#define PREFERENCES_MANAGER_H

#include <Preferences.h>
#include "config.h"

class PreferencesManager {
private:
    Preferences prefs;
public:
    PreferencesManager();
    bool begin();
    bool loadConfig(NetworkConfig &config);
    bool saveConfig(const NetworkConfig &config);
};

#endif // PREFERENCES_MANAGER_H