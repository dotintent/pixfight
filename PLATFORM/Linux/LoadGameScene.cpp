#include "LoadGameScene.hpp"
#include "glTextureLoader.hpp"

#ifdef __linux__
#include <dirent.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

LoadGameScene::LoadGameScene(const std::string & name, const std::string rootPath, struct nk_context *ctx) : BaseScene(name, rootPath, ctx) {

    _files.clear();
    _selectedFile = -1;
}

LoadGameScene::~LoadGameScene() {

}

SceneType LoadGameScene::Render(struct nk_font *smallfont, struct nk_font *normal) {

    _ctx->style.window.fixed_background = nk_style_item_image(_windowbg);
    if (nk_begin(_ctx, "Loadgame", nk_rect(0, 0, 1024, 768), NK_WINDOW_BACKGROUND)) {

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

        nk_layout_row_static(_ctx, 600, 1, 1);

        static const float ratio[] = {0.4, 0.3, 0.3};
        nk_layout_row(_ctx, NK_DYNAMIC, 80, 5, ratio);
        nk_spacing(_ctx, 1);

        if (nk_button_label(_ctx, "DELETE") && (_selectedFile != -1)) {

            std::string deletePath =  _rootPath + "save/" + _files[_selectedFile];

#ifdef __linux__
            unlink(deletePath.c_str());
#else
			_unlink(deletePath.c_str());
#endif

            _files.erase(_files.begin() + _selectedFile);
        }

        if (nk_button_label(_ctx, "LOAD") && (_selectedFile != -1)) {

            _type = SceneTypeRender;
        }

    }
    nk_end(_ctx);

    _ctx->style.window.fixed_background = nk_style_item_color(nk_rgb(100, 100, 100));

    _ctx->style.button.normal = nk_style_item_color(nk_rgba(0, 0, 0, 0));
    _ctx->style.button.hover  = nk_style_item_color(nk_rgba(0, 0, 0, 0));
    _ctx->style.button.active = nk_style_item_color(nk_rgba(245, 184, 48, 255));
    _ctx->style.button.border_color = nk_rgba(0,0,0,0);
    _ctx->style.button.text_background = nk_rgba(245, 184, 48, 255);

    if (nk_begin(_ctx, "Saves", nk_rect((1024 - 400) * 0.5, 15, 400, 500), NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {

        nk_style_set_font(_ctx, &smallfont->handle);

        for (int i = 0; i < _files.size(); ++i) {

            auto name = _files[i];

            nk_layout_row_static(_ctx, 50, 374, 1);

            if (nk_button_symbol_label(_ctx, (_selectedFile == i)?NK_SYMBOL_CIRCLE_OUTLINE:NK_SYMBOL_CIRCLE_SOLID, name.c_str(), NK_TEXT_CENTERED)) {

                _selectedFile = i;
            }
        }
    }
    nk_end(_ctx);

    nk_style_set_font(_ctx, &normal->handle);

    return _type;
}

void LoadGameScene::Init() {

    auto bgpath    = _rootPath + "menubg@2x.png";
    GLuint bgtex   = textureLoader.loadFile(bgpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _windowbg      = nk_subimage_id(bgtex, 960, 640, nk_rect(64, 0, 832, 640));

    auto btnhpath  = _rootPath + "back.png";
    GLuint btnhtex = textureLoader.loadFile(btnhpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _buttonhome    = nk_subimage_id(btnhtex, 32, 32, nk_rect(0, 0, 32, 32));

    auto btnnpath = _rootPath + "buttonN@2x.png";
    auto btnapath = _rootPath + "buttonP@2x.png";

    GLuint btnntex = textureLoader.loadFile(btnnpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    GLuint btnatex = textureLoader.loadFile(btnapath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

    _buttonnormal = nk_subimage_id(btnntex, 320, 80, nk_rect(0, 0, 320, 80));
    _buttonactive = nk_subimage_id(btnatex, 320, 80, nk_rect(0, 0, 320, 80));

    _files.clear();

#ifdef __linux__
    DIR *dpdf;
    struct dirent *epdf;

    std::string savePath = _rootPath + "save/";
    dpdf = opendir(savePath.c_str());

    if (dpdf != nullptr) {

        while (epdf = readdir(dpdf)){

            std::string filename(epdf->d_name);
            std::string::size_type idx = filename.rfind(".");

            if(idx != std::string::npos) {

                std::string extension = filename.substr(idx+1);
                if (extension != "sav") continue;
            }
            else {

                continue;
            }

            _files.push_back(filename);
        }
    }

    closedir(dpdf);

#else 

	std::string pattern = _rootPath + "save/";
	pattern.append("*.sav");
	WIN32_FIND_DATA data;
	HANDLE hFind;

	wchar_t* wString = new wchar_t[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, pattern.c_str(), -1, wString, MAX_PATH);

	if ((hFind = FindFirstFile(wString, &data)) != INVALID_HANDLE_VALUE) {
		
		do {

			size_t wcsChars = wcslen(data.cFileName);
			int size_needed = WideCharToMultiByte(CP_UTF8, 0, data.cFileName, wcsChars, NULL, 0, NULL, NULL);
			std::string path(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, data.cFileName, wcsChars, &path[0], size_needed, NULL, NULL);

			_files.push_back(path);
		} 
		while (FindNextFile(hFind, &data) != 0);
		
		FindClose(hFind);
	}

	delete wString;

#endif

    if (_files.size() > 0) {
        _selectedFile = 0;
    }
}

void LoadGameScene::Destroy() {

    _type = SceneTypeNone;
}

std::string LoadGameScene::getSelectedSave() {

    if (_selectedFile == -1) {
        return "";
    }

    if (_selectedFile > _files.size()) {
        return "";
    }

    return _files[_selectedFile];
}
