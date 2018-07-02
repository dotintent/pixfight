#pragma once

#include <memory>
#include <vector>

#include "BaseScene.hpp"
#include "PFMultiplayerClient.hpp"

class JoinRoomScene final : public BaseScene {

public:

    JoinRoomScene(const std::string & name, const std::string rootPath, struct nk_context *ctx);
    virtual ~JoinRoomScene() noexcept = default;

    JoinRoomScene(const JoinRoomScene & other) = delete;
    JoinRoomScene(JoinRoomScene && other) noexcept = delete;

    JoinRoomScene & operator= (const JoinRoomScene & other) = delete;
    JoinRoomScene & operator= (JoinRoomScene && other) noexcept = delete;

    virtual SceneType Render(struct nk_font *smallfont, struct nk_font *normal) override;

    virtual void Init() override;
    virtual void Destroy() override;

    std::weak_ptr<PFMultiplayerClient> client;

private:

    GLuint _bgtex;
    GLuint _btnntex;
    GLuint _btnatex;
    GLuint _btnhtex;

    struct nk_image _windowbg;
    struct nk_image _buttonnormal;
    struct nk_image _buttonactive;
    struct nk_image _buttonhome;

    std::vector<PFRoomInfo> _data;
    char _room[20];
    bool _error;
};
