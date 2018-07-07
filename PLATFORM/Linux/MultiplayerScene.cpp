#include "MultiplayerScene.hpp"
#include "glTextureLoader.hpp"
#include "ApplicationRunLoop.hpp"

using namespace std;

MultiplayerScene::MultiplayerScene(const std::string & name, const std::string rootPath, struct nk_context *ctx)
: BaseScene(name, rootPath, ctx) {

}

SceneType MultiplayerScene::Render(struct nk_font *smallfont, struct nk_font *normal) {

    if (!_connected) {

        _ctx->style.window.fixed_background = nk_style_item_color(nk_rgba(64, 64, 64, 255));

        nk_style_set_font(_ctx, &smallfont->handle);
        if (nk_begin(_ctx, "Error", nk_rect((1024 - 400) * 0.5, (768-150) * 0.5, 400, 150), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_BACKGROUND)) {

            nk_layout_row_static(_ctx, 30, 370, 1);
            nk_label(_ctx, "Could not connect to server, please try again.", NK_TEXT_ALIGN_CENTERED);

            static const float ratio[] = {0.3f, 0.4f, 0.3f};
            nk_layout_row(_ctx, NK_DYNAMIC, 40, 3, ratio);

            nk_spacing(_ctx, 1);
            if (nk_button_label(_ctx, "OK")) {

                _type = SceneTypeMenu;
            }

        }
        nk_end(_ctx);

        _ctx->style.window.fixed_background = nk_style_item_image(_windowbg);
        if (nk_begin(_ctx, "Multi", nk_rect(0, 0, 1024, 768), NK_WINDOW_BACKGROUND)) {

        }
        nk_end(_ctx);

        nk_style_set_font(_ctx, &normal->handle);
        return _type;
    }

    //background
    _ctx->style.window.fixed_background = nk_style_item_image(_windowbg);
    if (nk_begin(_ctx, "Multi", nk_rect(0, 0, 1024, 768), NK_WINDOW_BACKGROUND)) {

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

        _ctx->style.button.normal = nk_style_item_image(_buttonnormal);
        _ctx->style.button.hover  = nk_style_item_image(_buttonactive);
        _ctx->style.button.active = nk_style_item_image(_buttonactive);

        static const float ratio[] = {0.35f, 0.3f, 0.35f};

        nk_layout_row_static(_ctx, 300, 100, 1);
        nk_layout_row(_ctx, NK_DYNAMIC, 80, 3, ratio);
        nk_spacing(_ctx, 1);
        if (nk_button_label(_ctx, "MAKE ROOM")) {

            if (auto connection = client.lock()) {

                connection->makeRoom(_privateRoom);
            }
        }
        nk_spacing(_ctx, 1);

        nk_spacing(_ctx, 1);
        if (nk_checkbox_label(_ctx, "PRIVATE ROOM", &_privateRoom)) {

            std::cout << "BOOM: " << _privateRoom << std::endl;

        }
        nk_spacing(_ctx, 1);

        nk_spacing(_ctx, 1);
        if (nk_button_label(_ctx, "JOIN ROOM")) {

            _type = SceneTypeJoinRoom;
        }
        nk_spacing(_ctx, 1);
    }
    nk_end(_ctx);

    return _type;
}

void MultiplayerScene::Init() {

    _privateRoom = 1;
    _connected = false;

    if (auto connection = client.lock()) {

        _connected = connection->connect();

        connection->callback = [=](const PFSocketCommandType command, const vector<uint8_t> data){

            if (command != PFSocketCommandTypeMakeRoom) {
                return;
            }


            syncToRunLoop([&](){

                _type = SceneTypeMakeRoom;
            });
        };
    }

    auto bgpath = _rootPath + "menubg@2x.png";
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

    _ctx->style.checkbox.normal = nk_style_item_color(nk_rgba(245, 184, 48, 255));
    _ctx->style.checkbox.hover  = nk_style_item_color(nk_rgba(0, 0, 0, 255));
    _ctx->style.checkbox.active = nk_style_item_color(nk_rgba(245, 184, 48, 255));

}

void MultiplayerScene::Destroy() {

    _bgtex = 0;
    _btnntex = 0;
    _btnatex = 0;
    _btnhtex = 0;

    _connected = false;

    if (auto connection = client.lock()) {

        connection->callback = nullptr;
    }

    _type = SceneTypeNone;
}
