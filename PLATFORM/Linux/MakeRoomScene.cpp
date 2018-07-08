#include "MakeRoomScene.hpp"
#include "ApplicationRunLoop.hpp"
#include "glTextureLoader.hpp"

using namespace std;

MakeRoomScene::MakeRoomScene(const std::string & name, const std::string rootPath, struct nk_context *ctx)
: BaseScene(name, rootPath, ctx) {


}

void MakeRoomScene::finish() {

     if (auto connection = client.lock()) {

        if (isMaster) {

            connection->removeRoom();
        }
        else {

            connection->leaveRoom();
        }
    }

    _type = SceneTypeMenu;
}

SceneType MakeRoomScene::Render(struct nk_font *smallfont, struct nk_font *normal) {

    _ctx->style.window.fixed_background = nk_style_item_image(_windowbg);
    if (nk_begin(_ctx, "room", nk_rect(0, 0, 1024, 768), NK_WINDOW_BACKGROUND)) {

        _ctx->style.button.normal = nk_style_item_image(_buttonhome);
        _ctx->style.button.hover  = nk_style_item_image(_buttonhome);
        _ctx->style.button.active = nk_style_item_image(_buttonhome);

        nk_layout_row_static(_ctx, 5, 10, 0);
        nk_layout_row_begin(_ctx, NK_STATIC, 32, 3);

        //back button
        nk_layout_row_push(_ctx, 32);
        if (nk_button_label(_ctx, "")) {

            finish();
        }

        nk_layout_row_push(_ctx, 300);
        nk_spacing(_ctx, 1);

        nk_layout_row_push(_ctx, 300);

        if (isMaster) {

            if (nk_combo_begin_label(_ctx, _mapname.c_str(), nk_vec2(300, 200))) {
                nk_layout_row_dynamic(_ctx, 35, 1);

                for (int i = 0; i < 5; ++i) {

                    if (nk_combo_item_label(_ctx, _info.at(i).second.c_str(), NK_TEXT_CENTERED)) {

                        updateForMap(i);
                    }
                }

                nk_combo_end(_ctx);
            }

        }
        else {

            nk_spacing(_ctx, 1);
        }

        nk_layout_row_end(_ctx);
        nk_layout_row_static(_ctx, 10, 1, 1);
        nk_spacing(_ctx, 1);

        _ctx->style.button.normal = _currentmap;
        _ctx->style.button.hover  = _currentmap;
        _ctx->style.button.active = _currentmap;

        static const float ratio[] = {0.375f, 0.25f, 0.375f};
        nk_layout_row(_ctx, NK_DYNAMIC, 256, 3, ratio);

        nk_spacing(_ctx, 1);
        nk_button_label(_ctx, "");

        nk_layout_row_static(_ctx, 50, 1, 1);
        nk_spacing(_ctx, 1);

        nk_layout_row_static(_ctx, 20, 1000, 1);

        nk_style_set_font(_ctx, &smallfont->handle);
        if (auto connection = client.lock()) {

            char buff[100];
            sprintf(buff, "Room ID: %d", connection->getCurrentPort());
            nk_label(_ctx, buff, NK_TEXT_ALIGN_CENTERED);

        }
        else {

            nk_spacing(_ctx, 1);
        }

        nk_layout_row_static(_ctx, 20, 1000, 1);
        nk_label(_ctx, "Game will start automatically when all players are ready", NK_TEXT_ALIGN_CENTERED);

        nk_style_set_font(_ctx, &normal->handle);

        _ctx->style.button.normal = nk_style_item_image(_buttonnormal);
        _ctx->style.button.hover  = nk_style_item_image(_buttonactive);
        _ctx->style.button.active = nk_style_item_image(_buttonactive);

        nk_layout_row_static(_ctx, 60, 1, 1);
        nk_spacing(_ctx, 1);

        static const float buttonratio[] = {0.79375f, 0.15625f, 0.05f};
        nk_layout_row(_ctx, NK_DYNAMIC, 40, 3, buttonratio);
        nk_spacing(_ctx, 1);

        if (_ready) {

            if (nk_button_label(_ctx, "READY")) {

                _ready = false;

                if (auto connection = client.lock()) {

                    connection->setReady();
                }
            }
        }
        else {

            nk_spacing(_ctx, 1);
        }

        nk_spacing(_ctx, 1);
    }
    nk_end(_ctx);

    return _type;
}

void MakeRoomScene::Init() {

    _ready = false;
    _currentPlayerID = 1;

    string textures [] = {

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

    _info.insert(make_pair(0, make_pair(2, "tutorial")));
    _info.insert(make_pair(1, make_pair(3, "Hunt")));
    _info.insert(make_pair(2, make_pair(2, "pasage")));
    _info.insert(make_pair(3, make_pair(2, "waterway")));
    _info.insert(make_pair(4, make_pair(4, "kingofhill")));

    _currentmap = _maps[0];

    if (auto connection = client.lock()) {

        connection->callback = [=](const PFSocketCommandType command, const vector<uint8_t> data){

            syncToRunLoop([&, data, command](){

                if (command == PFSocketCommandTypeGameInfo) {

                    PFRoomInfo info;
                    memcpy(&info, data.data(), data.size() * sizeof(uint8_t));

                    connection->setRoomInfo(info);

                    _mapname = info.mapname;

                    int idx = 0;
                    for (auto it : _info) {

                        if (_mapname == it.second.second) {

                            _currentmap = _maps[idx];
                            break;
                        }

                        idx++;
                    }

                    _ready = true;
                }

                if (command == PFSocketCommandTypeLoad) {

                    uint32_t playerId = 0;
                    memcpy(&playerId, data.data(), sizeof(uint32_t));

                    _currentPlayerID = playerId + 1;
                    _type = SceneTypeRender;
                }

                if (command == PFSocketCommandTypeDisconnect) {

                    finish();
                }

            });
        };

        if (isMaster) {

            updateForMap(0);

        }
        else {

            connection->getRoomDetails();
        }
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

void MakeRoomScene::Destroy() {

    _bgtex = 0;
    _btnntex = 0;
    _btnatex = 0;
    _btnhtex = 0;

    _type = SceneTypeNone;

    _maps.clear();
    _info.clear();
}

void MakeRoomScene::updateForMap(int mapID) {

    if (auto connection = client.lock()) {

        auto mapdata = _info.at(mapID);

        _mapname = mapdata.second;
        _currentmap = _maps[mapID];

        PFRoomInfo roomInfo;

        strcpy(roomInfo.mapname, _mapname.c_str());
        roomInfo.players = mapdata.first;
        roomInfo.createdDate = time(0);
        roomInfo.roomPort = connection->getCurrentPort();

        connection->setRoomInfo(roomInfo);
        connection->sendRoomDetails();

        _ready = true;
    }
}
