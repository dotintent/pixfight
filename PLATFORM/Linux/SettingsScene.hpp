#pragma once

#include "BaseScene.hpp"
#include "Audio.hpp"

class SettingsScene final : public BaseScene {

public:

    SettingsScene(const std::string & name, const std::string rootPath, struct nk_context *ctx);
    virtual ~SettingsScene() noexcept;

    SettingsScene(const SettingsScene & other) = delete;
    SettingsScene(SettingsScene && other) noexcept = delete;

    SettingsScene & operator= (const SettingsScene & other) = delete;
    SettingsScene & operator= (SettingsScene && other) noexcept = delete;

    virtual SceneType Render(struct nk_font *small, struct nk_font *normal) override;

    virtual void Init() override;
    virtual void Destroy() override;

    void setAudio(Audio *audio) { _audio = audio; }

private:

    struct nk_image _windowbg;
    struct nk_image _buttonhome;

    Audio *_audio;
};
