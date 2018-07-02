#include "JoinRoomScene.hpp"
#include "glTextureLoader.hpp"
#include "ApplicationRunLoop.hpp"

using namespace std;

JoinRoomScene::JoinRoomScene(const std::string & name, const std::string rootPath, struct nk_context *ctx)
: BaseScene(name, rootPath, ctx) {

    memset(_room, 0, 20);
}

SceneType JoinRoomScene::Render(struct nk_font *smallfont, struct nk_font *normal) {

    if (_error) {

        _ctx->style.window.fixed_background = nk_style_item_color(nk_rgba(64, 64, 64, 255));

        nk_style_set_font(_ctx, &smallfont->handle);
        if (nk_begin(_ctx, "Error", nk_rect((1024 - 400) * 0.5, (768-150) * 0.5, 400, 150), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_BACKGROUND)) {

            nk_layout_row_static(_ctx, 30, 370, 1);
            nk_label(_ctx, "Could not connect to room.", NK_TEXT_ALIGN_CENTERED);

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

     _ctx->style.window.fixed_background = nk_style_item_image(_windowbg);
    if (nk_begin(_ctx, "join", nk_rect(0, 0, 1024, 768), NK_WINDOW_BACKGROUND)) {

        _ctx->style.button.normal = nk_style_item_image(_buttonhome);
        _ctx->style.button.hover  = nk_style_item_image(_buttonhome);
        _ctx->style.button.active = nk_style_item_image(_buttonhome);

        nk_layout_row_static(_ctx, 5, 10, 0);
        nk_layout_row_begin(_ctx, NK_STATIC, 32, 4);

        //back button
        nk_layout_row_push(_ctx, 32);
        if (nk_button_label(_ctx, "")) {

            _type = SceneTypeMenu;
        }

        nk_layout_row_push(_ctx, 310);
        nk_spacing(_ctx, 1);

        nk_layout_row_push(_ctx, 190);
        nk_label(_ctx, "ENTER CODE", NK_TEXT_ALIGN_LEFT);

        nk_layout_row_push(_ctx, 100);

        nk_style_set_font(_ctx, &smallfont->handle);
        nk_edit_string_zero_terminated(_ctx, NK_EDIT_FIELD, _room, 20, nk_filter_decimal);

        nk_layout_row_end(_ctx);

        nk_layout_row_static(_ctx, 10, 1, 1);
        nk_spacing(_ctx, 1);

        static float ratio[] = {0.34f, 0.3125f};
        nk_layout_row(_ctx, NK_DYNAMIC, 400, 2, ratio);

        _ctx->style.button.normal = nk_style_item_color(nk_rgb(64, 64, 64));
        _ctx->style.button.hover  = nk_style_item_color(nk_rgb(64, 64, 64));
        _ctx->style.button.active = nk_style_item_color(nk_rgb(64, 64, 64));

        nk_spacing(_ctx, 1);
        _ctx->style.window.fixed_background = nk_style_item_color(nk_rgba(64, 64, 64, 255));
        if (nk_group_begin(_ctx, "Rooms", NK_WINDOW_BORDER)) {

            nk_layout_row_static(_ctx, 40, 280, 1);

            for (int i = 0; i < _data.size(); ++i) {

                PFRoomInfo info = _data[i];

                struct tm * timeinfo;
                char buffer[30];
                timeinfo = localtime(&info.createdDate);
				strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M", timeinfo);

                std::string str(info.mapname);

                str += " - ";
                str += buffer;

                if (nk_button_label(_ctx, str.c_str())) {

                    //selected slot
                    std::string roomstr = std::to_string(info.roomPort);
                    strcpy(_room, roomstr.c_str());
                }
            }

        }
        nk_group_end(_ctx);

        nk_style_set_font(_ctx, &normal->handle);

        nk_layout_row_static(_ctx, 90, 1, 1);
        nk_spacing(_ctx, 1);

        _ctx->style.button.normal = nk_style_item_image(_buttonnormal);
        _ctx->style.button.hover  = nk_style_item_image(_buttonactive);
        _ctx->style.button.active = nk_style_item_image(_buttonactive);

        static float btnratio[] = {0.68f, 0.15625f, 0.15625, 0.04f};
        nk_layout_row(_ctx, NK_DYNAMIC, 40, 4, btnratio);

        nk_spacing(_ctx, 1);

        if (strlen(_room) > 0) {

            if (nk_button_label(_ctx, "JOIN")) {

                if (auto connection = client.lock()) {

                    if (connection->joinRoom(stoi(_room))) {

                        _type = SceneTypeMakeRoom;
                    }
                    else {

                        _error = true;
                    }

                }
                else {

                   _error = true;
                }
            }

        } else {

            nk_spacing(_ctx, 1);
        }

        if (nk_button_label(_ctx, "REFRESH")) {

            //REFRESH
            _data.clear();
            memset(_room, 0, 20);

            if (auto connection = client.lock()) {

                connection->listRooms();
            }
        }

        nk_spacing(_ctx, 1);

    }
    nk_end(_ctx);

    return _type;
}

void JoinRoomScene::Init() {

    _error = false;
    memset(_room, 0 , 20);

    if (auto connection = client.lock()) {

        connection->callback = [=](const PFSocketCommandType command, const vector<uint8_t> data) {

            if (command != PFSocketCommandTypeRooms) {
                return;
            }

            syncToRunLoop([&, data](){

                size_t roomsSize = (data.size() * sizeof(uint8_t)) / sizeof(PFRoomInfo);

                vector<PFRoomInfo> rooms(roomsSize);

                memcpy(rooms.data(), data.data(), data.size() * sizeof(uint8_t));

                _data = rooms;
            });
        };

        connection->listRooms();
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
    _buttonnormal = nk_subimage_id(_btnntex, 160, 40, nk_rect(0, 0, 160, 40));
    _buttonactive = nk_subimage_id(_btnatex, 160, 40, nk_rect(0, 0, 160, 40));
    _buttonhome   = nk_subimage_id(_btnhtex, 32, 32, nk_rect(0, 0, 32, 32));

    _ctx->style.button.text_normal = nk_rgb(0, 0, 0);
    _ctx->style.button.text_hover  = nk_rgb(0, 0, 0);
    _ctx->style.button.text_active = nk_rgb(0, 0, 0);

    _ctx->style.checkbox.normal = nk_style_item_color(nk_rgba(245, 184, 48, 255));
    _ctx->style.checkbox.hover  = nk_style_item_color(nk_rgba(0, 0, 0, 255));
    _ctx->style.checkbox.active = nk_style_item_color(nk_rgba(245, 184, 48, 255));
}

void JoinRoomScene::Destroy() {

    _bgtex = 0;
    _btnntex = 0;
    _btnatex = 0;
    _btnhtex = 0;

    _type = SceneTypeNone;

    _data.clear();
}
