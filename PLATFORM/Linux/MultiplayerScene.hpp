#pragma once

#include <memory>
#include "BaseScene.hpp"
#include "PFMultiplayerClient.hpp"

class MultiplayerScene final : public BaseScene {
public:

    MultiplayerScene(const std::string & name, const std::string rootPath, struct nk_context *ctx);
    virtual ~MultiplayerScene() noexcept = default;

    MultiplayerScene(const MultiplayerScene & other) = delete;
    MultiplayerScene(MultiplayerScene && other) noexcept = delete;

    MultiplayerScene & operator= (const MultiplayerScene & other) = delete;
    MultiplayerScene & operator= (MultiplayerScene && other) noexcept = delete;

    virtual SceneType Render(struct nk_font *smallfont, struct nk_font *normal) override;

    virtual void Init() override;
    virtual void Destroy() override;

public:

    std::weak_ptr<PFMultiplayerClient> client;

private:

    int _privateRoom;
    bool _connected;

    GLuint _bgtex;
    GLuint _btnntex;
    GLuint _btnatex;
    GLuint _btnhtex;

    struct nk_image _windowbg;
    struct nk_image _buttonnormal;
    struct nk_image _buttonactive;
    struct nk_image _buttonhome;
};


