#pragma once

#include "BaseScene.hpp"

class RenderScene final : public BaseScene {

public:

    RenderScene(const std::string & name, const std::string rootPath, struct nk_context *ctx);
    virtual ~RenderScene() noexcept;

    RenderScene(const RenderScene & other) = delete;
    RenderScene(RenderScene && other) noexcept = delete;

    RenderScene & operator= (const RenderScene & other) = delete;
    RenderScene & operator= (RenderScene && other) noexcept = delete;

    virtual SceneType Render() override;

    virtual void Init() override;
    virtual void Destroy() override;

    virtual bool isRender() override { return true; }

private:


};
