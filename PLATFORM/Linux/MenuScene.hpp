#pragma once

#include "BaseScene.hpp"

class MenuScene final : public BaseScene {

public:

    MenuScene(const std::string & name, const std::string rootPath, struct nk_context *ctx);
    virtual ~MenuScene() noexcept;

    MenuScene(const MenuScene & other) = delete;
    MenuScene(MenuScene && other) noexcept = delete;

    MenuScene & operator= (const MenuScene & other) = delete;
    MenuScene & operator= (MenuScene && other) noexcept = delete;

    virtual SceneType Render(struct nk_font *small, struct nk_font *normal) override;

    virtual void Init() override;
    virtual void Destroy() override;

private:

    GLuint _bgtex;
    GLuint _logtex;
    GLuint _btnntex;
    GLuint _btnatex;

    struct nk_image _windowbg;
    struct nk_image _logo;
    struct nk_image _buttonnormal;
    struct nk_image _buttonactive;
};
