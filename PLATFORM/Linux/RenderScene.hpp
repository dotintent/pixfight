#pragma once

#include "BaseScene.hpp"
#include "GameLogic.hpp"

class RenderScene final : public BaseScene {

public:

    RenderScene(const std::string & name, const std::string rootPath, struct nk_context *ctx);
    virtual ~RenderScene() noexcept;

    RenderScene(const RenderScene & other) = delete;
    RenderScene(RenderScene && other) noexcept = delete;

    RenderScene & operator= (const RenderScene & other) = delete;
    RenderScene & operator= (RenderScene && other) noexcept = delete;

    virtual SceneType Render(struct nk_font *smallfont, struct nk_font *normal) override;

    virtual void Init() override;
    virtual void Destroy() override;

    virtual bool isRender() override { return true; }

    virtual void handleScroll(double s) override;
    virtual void handleMouse(int button, int action, double x, double y) override;

    void setAudio(Audio *audio) { _audio = audio; }

    void newGame(std::string mapname, int players, int playerID);
    void loadGame(std::string path);

private:

    void setup(int teamID);

    GameLogic *_gameLogic;
    Audio *_audio;

    struct nk_image _homebtn;
    struct nk_image _timebtn;
    struct nk_image _turnbtn;

    struct nk_image _infantry;
    struct nk_image _bazooka;
    struct nk_image _jeep;
    struct nk_image _tank;
    struct nk_image _artillery;

    int _time;
    bool _gamewin;
    bool _gamelost;
    bool _botsthinking;
    bool _baseselected;
    GameBase *_selectedBase;
    bool _homemenu;

    struct nk_image _buttonnormal;

    bool _saved;
    bool _error;
};
