#pragma once

#include <memory>
#include <map>

#include "BaseScene.hpp"
#include "PFMultiplayerClient.hpp"

class MakeRoomScene final : public BaseScene {

public:


    MakeRoomScene(const std::string & name, const std::string rootPath, struct nk_context *ctx);
    virtual ~MakeRoomScene() noexcept = default;

    MakeRoomScene(const MakeRoomScene & other) = delete;
    MakeRoomScene(MakeRoomScene && other) noexcept = delete;

    MakeRoomScene & operator= (const MakeRoomScene & other) = delete;
    MakeRoomScene & operator= (MakeRoomScene && other) noexcept = delete;

    virtual SceneType Render(struct nk_font *smallfont, struct nk_font *normal) override;

    virtual void Init() override;
    virtual void Destroy() override;

    int getCurrentPlayerID() const { return _currentPlayerID; }

    bool isMaster;
    std::weak_ptr<PFMultiplayerClient> client;

private:

    void updateForMap(int mapID);
    void finish();

private:

    GLuint _bgtex;
    GLuint _btnntex;
    GLuint _btnatex;
    GLuint _btnhtex;

    struct nk_image _windowbg;
    struct nk_image _buttonnormal;
    struct nk_image _buttonactive;
    struct nk_image _buttonhome;

    int _currentPlayerID;
    bool _ready;
    std::string _mapname;

    std::vector<struct nk_style_item> _maps;
    std::map<int, std::pair<int, std::string>> _info;
    struct nk_style_item _currentmap;
};
