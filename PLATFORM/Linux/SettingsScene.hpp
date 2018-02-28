#pragma once

#include "BaseScene.hpp"

class SettingsScene final : public BaseScene {

public:

    SettingsScene(const std::string & name, const std::string rootPath, struct nk_context *ctx);
    virtual ~SettingsScene() noexcept;

    SettingsScene(const SettingsScene & other) = delete;
    SettingsScene(SettingsScene && other) noexcept = delete;

    SettingsScene & operator= (const SettingsScene & other) = delete;
    SettingsScene & operator= (SettingsScene && other) noexcept = delete;

    virtual SceneType Render() override;

    virtual void Init() override;
    virtual void Destroy() override;

private:


};
