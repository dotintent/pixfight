#pragma once

#include "BaseScene.hpp"

class NewGameScene final : public BaseScene {

public:

    NewGameScene(const std::string & name, const std::string rootPath, struct nk_context *ctx);
    virtual ~NewGameScene() noexcept;

    NewGameScene(const NewGameScene & other) = delete;
    NewGameScene(NewGameScene && other) noexcept = delete;

    NewGameScene & operator= (const NewGameScene & other) = delete;
    NewGameScene & operator= (NewGameScene && other) noexcept = delete;

    virtual SceneType Render(struct nk_font *small, struct nk_font *normal) override;

    virtual void Init() override;
    virtual void Destroy() override;

    int getSelectedMap() const { return _selectedMap; }
    int getPlayersCount() const { return _playersPlaying; }
    int getSelectedPlayer() const { return _selectedPlayer; }

private:

    GLuint _bgtex;
    GLuint _btnntex;
    GLuint _btnatex;
    GLuint _btnhtex;

    struct nk_image _windowbg;

    struct nk_image _buttonnormal;
    struct nk_image _buttonactive;

    struct nk_image _buttonhome;

    std::vector<struct nk_style_item> _maps;
    int _selectedMap;
    int _playersPlaying;
    int _selectedPlayer;
};
