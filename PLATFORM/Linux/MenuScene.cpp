#include "MenuScene.hpp"
#include "glTextureLoader.hpp"

MenuScene::MenuScene(const std::string & name, const std::string rootPath, struct nk_context *ctx)
: BaseScene(name, rootPath, ctx) {

}

MenuScene::~MenuScene() {

}

SceneType MenuScene::Render(struct nk_font *smallfont, struct nk_font *normal) {

    //logo
    _ctx->style.window.fixed_background = nk_style_item_image(_logo);
    nk_begin(_ctx, "Logo", nk_rect(154, 60, 716, 188), NK_WINDOW_BACKGROUND);
    nk_end(_ctx);

    //background
    _ctx->style.window.fixed_background = nk_style_item_image(_windowbg);
    if (nk_begin(_ctx, "MainMenu", nk_rect(0, 0, 1024, 768), NK_WINDOW_BACKGROUND)) {

        //buttons
        static const float ratio[] = {0.35f, 0.3f, 0.35f};

        nk_layout_row_static(_ctx, 320, 100, 1);
        nk_layout_row(_ctx, NK_DYNAMIC, 80, 3, ratio);
        nk_spacing(_ctx, 1);
        if (nk_button_label(_ctx, "SKIRMISH")) {

            std::cout << "SKIRMISH" << std::endl;
            _type = SceneTypeNewGame;
        }
        nk_spacing(_ctx, 1);

        nk_spacing(_ctx, 1);
        if (nk_button_label(_ctx, "MULTI")) {

            std::cout << "MULTIPLAYER" << std::endl;
            _type = SceneTypeMultiplayer;
        }
        nk_spacing(_ctx, 1);

        nk_spacing(_ctx, 1);
        if (nk_button_label(_ctx, "LOAD GAME")) {

            std::cout << "LOAD GAME" << std::endl;
            _type = SceneTypeLoadGame;
        }
        nk_spacing(_ctx, 1);

        nk_spacing(_ctx, 1);
        if (nk_button_label(_ctx, "SETTINGS")) {

            std::cout << "SETTINGS" << std::endl;
            _type = SceneTypeSettings;
        }
        nk_spacing(_ctx, 1);

    }
    nk_end(_ctx);

    return _type;
}

void MenuScene::Init() {

    auto bgpath = _rootPath + "menubg@2x.png";
    auto logopath = _rootPath + "logo@2x.png";
    auto btnnpath = _rootPath + "buttonN@2x.png";
    auto btnapath = _rootPath + "buttonP@2x.png";

    _bgtex   = textureLoader.loadFile(bgpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _logtex  = textureLoader.loadFile(logopath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _btnntex = textureLoader.loadFile(btnnpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _btnatex = textureLoader.loadFile(btnapath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

    _windowbg     = nk_subimage_id(_bgtex, 960, 640, nk_rect(64, 0, 832, 640));
    _logo         = nk_subimage_id(_logtex, 716, 188, nk_rect(0, 0, 716, 188));
    _buttonnormal = nk_subimage_id(_btnntex, 320, 80, nk_rect(0, 0, 320, 80));
    _buttonactive = nk_subimage_id(_btnatex, 320, 80, nk_rect(0, 0, 320, 80));

    _ctx->style.button.normal = nk_style_item_image(_buttonnormal);
    _ctx->style.button.hover  = nk_style_item_image(_buttonactive);
    _ctx->style.button.active = nk_style_item_image(_buttonactive);

    _ctx->style.button.text_normal = nk_rgb(0, 0, 0);
    _ctx->style.button.text_hover  = nk_rgb(0, 0, 0);
    _ctx->style.button.text_active = nk_rgb(0, 0, 0);
}

void MenuScene::Destroy() {

    _bgtex = 0;
    _logtex = 0;
    _btnntex = 0;
    _btnatex = 0;

    _type = SceneTypeNone;
}

