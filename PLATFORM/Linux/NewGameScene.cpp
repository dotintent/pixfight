#include "NewGameScene.hpp"
#include "glTextureLoader.hpp"

NewGameScene::NewGameScene(const std::string & name, const std::string rootPath, struct nk_context *ctx)
: BaseScene(name, rootPath, ctx) {

}

NewGameScene::~NewGameScene() {

}

SceneType NewGameScene::Render(struct nk_font *smallfont, struct nk_font *normal) {

    static std::string teams[] = {

        "BLUE",
        "RED",
        "GREEN",
        "YELLOW"
    };

    //map preview
    _ctx->style.window.fixed_background = _maps[_selectedMap];
    if (nk_begin(_ctx, "mapimage", nk_rect(1024 - 272, 15, 256, 256), NK_WINDOW_BACKGROUND)) {
    }
    nk_end(_ctx);

    //root layout
    _ctx->style.window.fixed_background = nk_style_item_image(_windowbg);
    if (nk_begin(_ctx, "NewGame", nk_rect(0, 0, 1024, 768), NK_WINDOW_BACKGROUND)) {

        _ctx->style.button.normal = nk_style_item_image(_buttonhome);
        _ctx->style.button.hover  = nk_style_item_image(_buttonhome);
        _ctx->style.button.active = nk_style_item_image(_buttonhome);

        nk_layout_row_static(_ctx, 5, 10, 0);
        nk_layout_row_begin(_ctx, NK_STATIC, 32, 3);

        //back button
        nk_layout_row_push(_ctx, 32);
        if (nk_button_label(_ctx, "")) {

            _type = SceneTypeMenu;
        }

        //team select
        nk_layout_row_push(_ctx, 300);
        nk_spacing(_ctx, 1);

        if (nk_combo_begin_label(_ctx, teams[_selectedPlayer].c_str(), nk_vec2(300, 200))) {
            nk_layout_row_dynamic(_ctx, 35, 1);

            for (int i =0; i<_playersPlaying; ++i) {

                if (nk_combo_item_label(_ctx, teams[i].c_str(), NK_TEXT_CENTERED)) {

                    _selectedPlayer = i;
                }
            }

            nk_combo_end(_ctx);
        }

        nk_layout_row_push(_ctx, 10);
        nk_layout_row_end(_ctx);

        //maps style
        _ctx->style.button.normal = nk_style_item_color(nk_rgba(0, 0, 0, 0));
        _ctx->style.button.hover  = nk_style_item_color(nk_rgba(0, 0, 0, 0));
        _ctx->style.button.active = nk_style_item_color(nk_rgba(245, 184, 48, 255));
        _ctx->style.button.border_color = nk_rgba(0,0,0,0);
        _ctx->style.button.text_background = nk_rgba(245, 184, 48, 255);

        //maps
        static const float ratio[] = {0.3f, 0.4f, 0.3f};

        nk_layout_row(_ctx, NK_DYNAMIC, 80, 2, ratio);
        nk_spacing(_ctx, 1);

        if (nk_button_symbol_label(_ctx, (_selectedMap == 0)?NK_SYMBOL_CIRCLE_OUTLINE:NK_SYMBOL_CIRCLE_SOLID, "TUTORIAL", NK_TEXT_CENTERED)) {

            _selectedMap = 0;
            _playersPlaying = 2;
            _selectedPlayer = 0;
        }

        nk_layout_row(_ctx, NK_DYNAMIC, 80, 2, ratio);
        nk_spacing(_ctx, 1);

        if (nk_button_symbol_label(_ctx, (_selectedMap == 1)?NK_SYMBOL_CIRCLE_OUTLINE:NK_SYMBOL_CIRCLE_SOLID, "HUNT", NK_TEXT_CENTERED)) {

            _selectedMap = 1;
            _playersPlaying = 3;
            _selectedPlayer = 0;
        }

        nk_layout_row(_ctx, NK_DYNAMIC, 80, 2, ratio);
        nk_spacing(_ctx, 1);

        if (nk_button_symbol_label(_ctx, (_selectedMap == 2)?NK_SYMBOL_CIRCLE_OUTLINE:NK_SYMBOL_CIRCLE_SOLID, "PASAGE", NK_TEXT_CENTERED)) {

            _selectedMap = 2;
            _playersPlaying = 2;
            _selectedPlayer = 0;
        }

        nk_layout_row(_ctx, NK_DYNAMIC, 80, 2, ratio);
        nk_spacing(_ctx, 1);

        if (nk_button_symbol_label(_ctx, (_selectedMap == 3)?NK_SYMBOL_CIRCLE_OUTLINE:NK_SYMBOL_CIRCLE_SOLID, "WATERWAY", NK_TEXT_CENTERED)) {

            _selectedMap = 3;
            _playersPlaying = 2;
            _selectedPlayer = 0;
        }

        nk_layout_row(_ctx, NK_DYNAMIC, 80, 2, ratio);
        nk_spacing(_ctx, 1);

        if (nk_button_symbol_label(_ctx, (_selectedMap == 4)?NK_SYMBOL_CIRCLE_OUTLINE:NK_SYMBOL_CIRCLE_SOLID, "KINGOFHILL", NK_TEXT_CENTERED)) {

            _selectedMap = 4;
            _playersPlaying = 4;
            _selectedPlayer = 0;
        }

        _ctx->style.button.normal = nk_style_item_image(_buttonnormal);
        _ctx->style.button.active = nk_style_item_image(_buttonactive);

        nk_layout_row_static(_ctx, 180, 100, 1);

        static const float sratio[] = {0.35f, 0.3f, 0.35f};

        nk_layout_row(_ctx, NK_DYNAMIC, 80, 2, sratio);
        nk_spacing(_ctx, 1);

        if (nk_button_label(_ctx, "START")) {

            _type = SceneTypeRender;
        }
    }
    nk_end(_ctx);

    return _type;
}

void NewGameScene::Init() {

    _selectedMap = 0;
    _playersPlaying = 2;
    _selectedPlayer = 0;

    auto bgpath   = _rootPath + "menubg@2x.png";
    auto btnnpath = _rootPath + "buttonN@2x.png";
    auto btnapath = _rootPath + "buttonP@2x.png";
    auto btnhpath = _rootPath + "back.png";

    _bgtex   = textureLoader.loadFile(bgpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _btnntex = textureLoader.loadFile(btnnpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _btnatex = textureLoader.loadFile(btnapath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _btnhtex = textureLoader.loadFile(btnhpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

    _windowbg     = nk_subimage_id(_bgtex, 960, 640, nk_rect(64, 0, 832, 640));

    _buttonnormal = nk_subimage_id(_btnntex, 320, 80, nk_rect(0, 0, 320, 80));
    _buttonactive = nk_subimage_id(_btnatex, 320, 80, nk_rect(0, 0, 320, 80));
    _buttonhome   = nk_subimage_id(_btnhtex, 32, 32, nk_rect(0, 0, 32, 32));

    _ctx->style.button.text_normal = nk_rgb(0, 0, 0);
    _ctx->style.button.text_hover  = nk_rgb(0, 0, 0);
    _ctx->style.button.text_active = nk_rgb(0, 0, 0);

    std::string textures [] = {

        "tutorial.png",
        "Hunt.png",
        "pasage.png",
        "waterway.png",
        "kingofhill.png"
    };

    for (int i = 0; i < 5; ++i) {

        auto path = _rootPath + textures[i];
        GLuint tex = textureLoader.loadFile(path, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

        auto subimage = nk_subimage_id(tex, 256, 256, nk_rect(0, 0, 256, 256));
        auto imageItem = nk_style_item_image(subimage);

        _maps.push_back(imageItem);
    }
}

void NewGameScene::Destroy() {

    _bgtex = 0;
    _btnntex = 0;
    _btnatex = 0;
    _btnhtex = 0;

    _maps.clear();

    _type = SceneTypeNone;
}
