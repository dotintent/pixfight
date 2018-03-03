#pragma once

#include "Core-pch.hpp"

class PFSettings final {
public:

    PFSettings(const std::string & path);
    virtual ~PFSettings() = default;

    void setMute(bool mute) {

        _settings.mute = mute;
        save();
    }

    void setHardai(bool ai) {

        _settings.hardai = ai;
        save();
    }

    bool getMute() {

        if (!load()) {
            return false;
        }

        return _settings.mute;
    }

    bool getHardai() {

        if (!load()) {
            return false;
        }

        return _settings.hardai;
    }

private:

    std::string _path;

    struct settings {
        bool mute;
        bool hardai;
    } _settings;

    void save();
    bool load();
};

extern PFSettings *GameSettings;
