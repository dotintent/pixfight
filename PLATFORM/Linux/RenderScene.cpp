#include "RenderScene.hpp"
#include "glTextureLoader.hpp"
#include "PFSettings.h"

RenderScene::RenderScene(const std::string & name, const std::string rootPath, struct nk_context *ctx)
: BaseScene(name, rootPath, ctx)
, _gameLogic(nullptr)
, _time(1)
, _gamewin(false)
, _gamelost(false)
, _botsthinking(false)
, _baseselected(false)
, _selectedBase(nullptr)
, _homemenu(false)
, _saved(false)
, _error(false) {

}

RenderScene::~RenderScene() {

}

SceneType RenderScene::Render(struct nk_font *smallfont, struct nk_font *normal) {

    //game render
    _gameLogic->Render();

    //ui
    _ctx->style.window.fixed_background = nk_style_item_color(nk_rgb(100, 100, 100));

    if (_saved) {

        if (nk_begin(_ctx, _error ? "ERROR" : "GAME SAVED", nk_rect((1024 - 600) * 0.5, (768 - 150) * 0.5, 600, 150), NK_WINDOW_CLOSABLE|NK_WINDOW_TITLE|NK_WINDOW_BORDER)) {

            nk_layout_row_static(_ctx, 20, 10, 1);
            nk_layout_row_static(_ctx, 40, 570, 1);
            nk_label(_ctx, _error ? "Game could not be saved" : "Game saved sucessfully", NK_TEXT_ALIGN_CENTERED);
        }
        else {

            _saved = false;
        }

        nk_end(_ctx);
    }

    if (_gamewin) {

        if (nk_begin(_ctx, "GAME WIN", nk_rect((1024 - 600) * 0.5, (768 - 150) * 0.5, 600, 150), NK_WINDOW_BACKGROUND|NK_WINDOW_TITLE|NK_WINDOW_BORDER)) {

            nk_layout_row_static(_ctx, 20, 10, 1);
            nk_layout_row_static(_ctx, 40, 570, 1);
            nk_label(_ctx, "Congratulations you win", NK_TEXT_ALIGN_CENTERED);
        }
        nk_end(_ctx);
    }

    if (_gamelost) {

        if (nk_begin(_ctx, "GAME LOST", nk_rect((1024 - 600) * 0.5, (768 - 150) * 0.5, 600, 150), NK_WINDOW_BACKGROUND|NK_WINDOW_TITLE|NK_WINDOW_BORDER)) {

            nk_layout_row_static(_ctx, 20, 10, 1);
            nk_layout_row_static(_ctx, 40, 570, 1);
            nk_label(_ctx, "Sorry you lose", NK_TEXT_ALIGN_CENTERED);
        }
        nk_end(_ctx);
    }

    if (_botsthinking) {

        if (nk_begin(_ctx, "BOTS", nk_rect((1024 - 400) * 0.5, (768 - 150) * 0.5, 400, 150), NK_WINDOW_BACKGROUND|NK_WINDOW_BORDER)) {

            nk_layout_row_static(_ctx, 20, 10, 1);
            nk_layout_row_static(_ctx, 40, 570, 1);
            nk_label(_ctx, "Thinking...", NK_TEXT_ALIGN_CENTERED);
        }
        nk_end(_ctx);
    }

    if  (_homemenu) {

        if (nk_begin(_ctx, "MENU", nk_rect((1024 - 186) * 0.5, (768 - 200) * 0.5, 186, 200), NK_WINDOW_CLOSABLE|NK_WINDOW_TITLE|NK_WINDOW_BORDER)) {

            _ctx->style.button.normal = nk_style_item_image(_buttonnormal);
            _ctx->style.button.hover  = nk_style_item_image(_buttonnormal);
            _ctx->style.button.active = nk_style_item_image(_buttonnormal);

            nk_layout_row_static(_ctx, 20, 10, 1);
            nk_layout_row_static(_ctx, 40, 160, 1);

            if (nk_button_label(_ctx, "QUIT")) {

                _type = SceneTypeMenu;
                _homemenu = false;
            }

            nk_layout_row_static(_ctx, 40, 160, 1);
            if (nk_button_label(_ctx, "SAVE")) {

                //get time

                time_t rawtime;
                struct tm * timeinfo;
                char buffer[255];

                time (&rawtime);
                timeinfo = localtime(&rawtime);

#ifdef _WIN32
				strftime(buffer, sizeof(buffer), "%m_%d_%Y_%I_%M_%S", timeinfo);
#else
                strftime(buffer, sizeof(buffer), "%m_%d_%Y_%I:%M:%S", timeinfo);
#endif

                std::string str(buffer);

                std::string fullpath = _rootPath + "save/" + _gameLogic->getCurrentMapName() + "_" + str + ".sav";

                _error = !_gameLogic->saveGame(fullpath);
				_saved = true;
                _homemenu = false;
            }

        }
        else {

            _homemenu = false;
        }

        nk_end(_ctx);
    }

    if (_baseselected && _selectedBase) {

        nk_style_set_font(_ctx, &smallfont->handle);

        int cash = _gameLogic->getPlayerCash();

        std::stringstream stream;
        stream << "BUILD, CASH: ";
        stream << cash;

        _ctx->style.button.normal = nk_style_item_color(nk_rgb(50, 50, 50));
        _ctx->style.button.hover  = nk_style_item_color(nk_rgb(200, 200, 200));
        _ctx->style.button.active = nk_style_item_color(nk_rgb(245, 184, 48));

        if (nk_begin(_ctx, stream.str().c_str(), nk_rect((1024 - 300) * 0.5, (768 - 420) * 0.5, 300, 420), NK_WINDOW_CLOSABLE|NK_WINDOW_TITLE|NK_WINDOW_BORDER)) {

            int selectedUnit = -1;
            int cost = 0;

            nk_layout_row_static(_ctx, 20, 10, 1);

            nk_layout_row_static(_ctx, 64, 274, 1);
            _ctx->style.button.text_normal = cash > 75 ? nk_rgb(0, 0, 0) : nk_rgb(255, 0, 0);
            if (nk_button_image_label(_ctx, _infantry, "Infantry - 75", NK_TEXT_CENTERED) && (cash > 75)) {

                selectedUnit = 0;
                cost = 75;
            }

            nk_layout_row_static(_ctx, 64, 274, 1);
            _ctx->style.button.text_normal = cash > 150 ? nk_rgb(0, 0, 0) : nk_rgb(255, 0, 0);
            if (nk_button_image_label(_ctx, _bazooka, "Bazooka - 150", NK_TEXT_CENTERED) && (cash > 150)) {

                selectedUnit = 1;
                cost = 150;
            }

            nk_layout_row_static(_ctx, 64, 274, 1);
            _ctx->style.button.text_normal = cash > 200 ? nk_rgb(0, 0, 0) : nk_rgb(255, 0, 0);
            if (nk_button_image_label(_ctx, _jeep, "Jeep - 200", NK_TEXT_CENTERED) && (cash > 200)) {

                selectedUnit = 2;
                cost = 200;
            }

            nk_layout_row_static(_ctx, 64, 274, 1);
            _ctx->style.button.text_normal = cash > 300 ? nk_rgb(0, 0, 0) : nk_rgb(255, 0, 0);
            if (nk_button_image_label(_ctx, _tank, "Tank - 300", NK_TEXT_CENTERED) && (cash > 300)) {

                selectedUnit = 3;
                cost = 300;
            }

            nk_layout_row_static(_ctx, 64, 274, 1);
            _ctx->style.button.text_normal = cash > 200 ? nk_rgb(0, 0, 0) : nk_rgb(255, 0, 0);
            if (nk_button_image_label(_ctx, _artillery, "Artillery - 200", NK_TEXT_CENTERED) && (cash > 200)) {

                selectedUnit = 4;
                cost = 200;
            }

            if (selectedUnit != -1) {

                _selectedBase->setUnitToBuild(selectedUnit);
                _gameLogic->setPlayerCash(cash - cost);
                _gameLogic->buildUnit(_selectedBase);
                _selectedBase = nullptr;

                _baseselected = false;
            }

        }
        else {

            _selectedBase = nullptr;
            _baseselected = false;
        }

        nk_end(_ctx);
        nk_style_set_font(_ctx, &normal->handle);
    }

    _ctx->style.button.text_normal = nk_rgb(0, 0, 0);

    _ctx->style.window.fixed_background = nk_style_item_color(nk_rgba(0, 0, 0, 0));
    if (nk_begin(_ctx, "", nk_rect(0, 0, 1024, 768), NK_WINDOW_BACKGROUND)) {

        _ctx->style.button.normal = nk_style_item_image(_homebtn);
        _ctx->style.button.hover  = nk_style_item_image(_homebtn);
        _ctx->style.button.active = nk_style_item_image(_homebtn);

        nk_layout_row_static(_ctx, 5, 10, 0);

        static const float ratio[] = {0.031f, 0.4535, 0.031f, 0.4535, 0.031f};
        nk_layout_row(_ctx, NK_DYNAMIC, 32, 5, ratio);

        //back button
        if (nk_button_label(_ctx, "") && !_saved) {

            _homemenu = true;
        }

        _ctx->style.button.normal = nk_style_item_image(_timebtn);
        _ctx->style.button.hover  = nk_style_item_image(_timebtn);
        _ctx->style.button.active = nk_style_item_image(_timebtn);

        nk_spacing(_ctx, 1);

        std::stringstream ss;

        ss << "X";
        ss << _time;

        nk_style_set_font(_ctx, &smallfont->handle);
        if (nk_button_label(_ctx, ss.str().c_str())) {

            _time = _gameLogic->multiplyTime();
        }
        nk_style_set_font(_ctx, &normal->handle);

        _ctx->style.button.normal = nk_style_item_image(_turnbtn);
        _ctx->style.button.hover  = nk_style_item_image(_turnbtn);
        _ctx->style.button.active = nk_style_item_image(_turnbtn);

        nk_spacing(_ctx, 1);
        if (nk_button_label(_ctx, "") && !_homemenu && !_baseselected) {

            if (_gameLogic->canEndTurn()) {

                _gameLogic->endTurn();
            }
        }

    }
    nk_end(_ctx);

    return _type;
}

void RenderScene::Init() {

    _gameLogic = new GameLogic(1024, 768, _rootPath, _audio);

    _gameLogic->context = this;

    _gameLogic->winGameCallback = [&](void* context) {

        _gamewin = true;
    };

    _gameLogic->loseGameCallback = [&](void* context) {

        _gamelost = true;
    };

    _gameLogic->botsStartThinkCallback = [&](void* context) {

        _botsthinking = true;
    };

    _gameLogic->botsEndThinkCallback = [&](void* context) {

        _botsthinking = false;
    };

    _gameLogic->baseSelectedCallback = [&](void* context, GameBase *base) {

        _baseselected = true;
        _selectedBase = base;
    };

    bool hardAI = GameSettings->getHardai();
    _gameLogic->setHardAI(hardAI);
}

void RenderScene::Destroy() {

    delete _gameLogic;
    _gameLogic = nullptr;

    _type = SceneTypeNone;
}

void RenderScene::newGame(std::string mapname, int players, int playerID) {

    int teamID = playerID+1;

    _gameLogic->createNewGame(mapname, teamID, players);

    setup(teamID);
}

void RenderScene::loadGame(std::string path) {

    _gameLogic->loadGame(path);

    setup(PLAYERTEAMSELECTED);
}

void RenderScene::setup(int teamID) {

    auto htexpath = _rootPath + "home.png";
    auto etexpath = _rootPath + "empty.png";
    auto ttexpath = _rootPath + "turn.png";

    GLuint htex = textureLoader.loadFile(htexpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    GLuint etex = textureLoader.loadFile(etexpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    GLuint ttex = textureLoader.loadFile(ttexpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

    _homebtn = nk_subimage_id(htex, 32, 32, nk_rect(0, 0, 32, 32));
    _timebtn = nk_subimage_id(etex, 32, 32, nk_rect(0, 0, 32, 32));
    _turnbtn = nk_subimage_id(ttex, 32, 32, nk_rect(0, 0, 32, 32));

    std::stringstream icon;

    icon << _rootPath + "icons/";
    icon << "icon_infantry";
    icon << teamID;
    icon << ".png";

    GLuint icon_inf = textureLoader.loadFile(icon.str(), GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _infantry = nk_subimage_id(icon_inf, 64, 64, nk_rect(0, 0, 64, 64));

    icon.str(std::string());

    icon << _rootPath + "icons/";
    icon << "icon_bazooka";
    icon << teamID;
    icon << ".png";

    GLuint icon_baz = textureLoader.loadFile(icon.str(), GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _bazooka = nk_subimage_id(icon_baz, 64, 64, nk_rect(0, 0, 64, 64));

    icon.str(std::string());

    icon << _rootPath + "icons/";
    icon << "icon_jeep";
    icon << teamID;
    icon << ".png";

    GLuint icon_jeep = textureLoader.loadFile(icon.str(), GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _jeep = nk_subimage_id(icon_jeep, 64, 64, nk_rect(0, 0, 64, 64));

    icon.str(std::string());

    icon << _rootPath + "icons/";
    icon << "icon_tank";
    icon << teamID;
    icon << ".png";

    GLuint icon_tank = textureLoader.loadFile(icon.str(), GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _tank = nk_subimage_id(icon_tank, 64, 64, nk_rect(0, 0, 64, 64));

    icon.str(std::string());

    icon << _rootPath + "icons/";
    icon << "icon_artillery";
    icon << teamID;
    icon << ".png";

    GLuint icon_alt = textureLoader.loadFile(icon.str(), GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _artillery = nk_subimage_id(icon_alt, 64, 64, nk_rect(0, 0, 64, 64));

    auto btnnpath = _rootPath + "buttonN.png";
    GLuint btnntex = textureLoader.loadFile(btnnpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

    _buttonnormal = nk_subimage_id(btnntex, 160, 40, nk_rect(0, 0, 160, 40));
}

void RenderScene::handleScroll(double s) {

    if (_gameLogic == nullptr) {
        return;
    }

    float scale = _gameLogic->getCurrentScale();
    scale += s*0.05;

    _gameLogic->setCurrentScale(scale);
}

void RenderScene::handleMouse(int button, int action, double x, double y) {

    if (_gameLogic == nullptr) {
        return;
    }

    static xVec2 initialPos;
    static bool dragging;

    if (button == 0 && action == 1) {

        if (!_baseselected) {

            _gameLogic->touchDownAtPoint(xVec2(x, y));
        }
    }
    else if (button == 1 && action == 1) {

        dragging = true;
        initialPos = xVec2(x, y);
    }
    else if (button == 1 && action == 0) {

        dragging = false;
    }

    if (dragging) {

        xVec2 cPos = xVec2(x, y);
        xVec2 dir = (cPos - initialPos);

        dir.normalize();
        dir = dir * 240.0f;

        _gameLogic->setDirectionVec(dir);
    }
}
