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

    virtual SceneType Render(struct nk_font *small, struct nk_font *normal) override;

    virtual void Init() override;
    virtual void Destroy() override;

private:


};
