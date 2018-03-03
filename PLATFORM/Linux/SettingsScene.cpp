#include "SettingsScene.hpp"
#include "glTextureLoader.hpp"
#include "PFSettings.h"

SettingsScene::SettingsScene(const std::string & name, const std::string rootPath, struct nk_context *ctx) : BaseScene(name, rootPath, ctx) {

    _audio = nullptr;
}

SettingsScene::~SettingsScene() {

}

SceneType SettingsScene::Render(struct nk_font *small, struct nk_font *normal) {

    _ctx->style.window.fixed_background = nk_style_item_image(_windowbg);
    if (nk_begin(_ctx, "Settings", nk_rect(0, 0, 1024, 768), NK_WINDOW_BACKGROUND)) {

        _ctx->style.button.normal = nk_style_item_image(_buttonhome);
        _ctx->style.button.hover  = nk_style_item_image(_buttonhome);
        _ctx->style.button.active = nk_style_item_image(_buttonhome);

        nk_layout_row_static(_ctx, 5, 10, 0);
        nk_layout_row_begin(_ctx, NK_STATIC, 32, 1);

        //back button
        nk_layout_row_push(_ctx, 32);
        if (nk_button_label(_ctx, "")) {

            _type = SceneTypeMenu;
        }

        nk_layout_row_end(_ctx);

        enum {ENABLED, DISABLED};
        static int snd = GameSettings->getMute() ? ENABLED : DISABLED;
        static int hai = GameSettings->getHardai() ? ENABLED : DISABLED;

        _ctx->style.text.color = nk_rgb(0,0,0);

        _ctx->style.option.text_normal = nk_rgb(0,0,0);
        _ctx->style.option.text_active = nk_rgba(245, 184, 48, 255);

        _ctx->style.option.cursor_normal = nk_style_item_color(nk_rgba(245, 184, 48, 255));
        _ctx->style.option.cursor_hover  = nk_style_item_color(nk_rgba(245, 184, 48, 255));

        nk_layout_row_static(_ctx, 300, 1, 1);

        static const float ratio[] = {0.22f, 0.16, 0.2, 0.2, 0.22f};
        nk_layout_row(_ctx, NK_DYNAMIC, 32, 5, ratio);
        nk_spacing(_ctx, 1);
        nk_label(_ctx, "MUTE ", NK_TEXT_ALIGN_LEFT);

        if (nk_option_label(_ctx, "Enaled", snd == ENABLED)) {

            snd = ENABLED;
            GameSettings->setMute(true);
            _audio->mute();
        }

        if (nk_option_label(_ctx, "Disabled", snd == DISABLED)) {

            snd = DISABLED;
            GameSettings->setMute(false);
            _audio->unmute();
        }

        nk_spacing(_ctx, 1);

        static const float ratioai[] = {0.22f, 0.16, 0.2, 0.2, 0.22f};
        nk_layout_row(_ctx, NK_DYNAMIC, 32, 5, ratioai);
        nk_spacing(_ctx, 1);
        nk_label(_ctx, "HARD AI ", NK_TEXT_ALIGN_LEFT);

        if (nk_option_label(_ctx, "Enaled", hai == ENABLED)) {

            hai = ENABLED;
            GameSettings->setHardai(true);
        }

        if (nk_option_label(_ctx, "Disabled", hai == DISABLED)) {

            hai = DISABLED;
            GameSettings->setHardai(false);
        }

        nk_spacing(_ctx, 1);

    }
    nk_end(_ctx);

    return _type;
}

void SettingsScene::Init() {

    auto bgpath    = _rootPath + "menubg@2x.png";
    GLuint bgtex   = textureLoader.loadFile(bgpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _windowbg      = nk_subimage_id(bgtex, 960, 640, nk_rect(64, 0, 832, 640));

    auto btnhpath  = _rootPath + "back.png";
    GLuint btnhtex = textureLoader.loadFile(btnhpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _buttonhome    = nk_subimage_id(btnhtex, 32, 32, nk_rect(0, 0, 32, 32));
}

void SettingsScene::Destroy() {

    _type = SceneTypeNone;
}

