#pragma once

#include "BaseScene.hpp"

class LoadGameScene final : public BaseScene {

public:

    LoadGameScene(const std::string & name, const std::string rootPath, struct nk_context *ctx);
    virtual ~LoadGameScene() noexcept;

    LoadGameScene(const LoadGameScene & other) = delete;
    LoadGameScene(LoadGameScene && other) noexcept = delete;

    LoadGameScene & operator= (const LoadGameScene & other) = delete;
    LoadGameScene & operator= (LoadGameScene && other) noexcept = delete;

    virtual SceneType Render(struct nk_font *smallfont, struct nk_font *normal) override;

    virtual void Init() override;
    virtual void Destroy() override;

    std::string getSelectedSave();

private:

    struct nk_image _windowbg;
    struct nk_image _buttonhome;
    std::vector<std::string> _files;

    struct nk_image _buttonnormal;
    struct nk_image _buttonactive;

    int _selectedFile;
};
