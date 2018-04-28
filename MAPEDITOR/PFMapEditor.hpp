//
//  PFMapEditor.hpp
//  PixEditor
//
//  Created by Marcin Małysz on 24/04/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING

#include <stdarg.h>
#include "nuklear.h"
#include "xVec2.h"
#include "GameMap.hpp"
#include "GameUnit.hpp"
#include "GameBase.hpp"

#include <GLFW/glfw3.h>

typedef PF_ENUM(unsigned int, PFLayoutMode) {

    PFLayoutModeTiles,
    PFLayoutModeUnits
};

typedef PF_ENUM(unsigned int, PFPenMode) {
    PFPenModeNone,
    PFPenModeDrawing,
    PFPenModeClear,
    PFPenModeLock,
    PFPenModeUnlock,
    PFPenModeAddUnit,
    PFPenModeAddBase,
    PFPenModeDeleteObj
};

class PFMapEditor final {

    constexpr static const float MaxFrameTime = 0.15;
    constexpr static const int MaxTilesCount = 20;

public:

    PFMapEditor(GLFWwindow *win, const std::string &rootpath, struct nk_context *ctx);
    ~PFMapEditor() noexcept;

    PFMapEditor(const PFMapEditor & other) = delete;
    PFMapEditor(PFMapEditor && other) noexcept = delete;

    PFMapEditor & operator= (const PFMapEditor & other) = delete;
    PFMapEditor & operator= (PFMapEditor && other) noexcept = delete;

    void handleScroll(double xoff, double yoff);
    void handleMouse(double x, double y, int button, int action);

    void Render();
    bool loadBlank();

private:

    void release();
    void renderInterface();

    bool load(const std::string &path);
    bool save(const std::string &path);

    bool loadLogic(const std::string &path);
    bool saveLogic(const std::string &path);

    void makeLayerImages();
    void addUnit(const xVec2 &pos);
    void addBase(const xVec2 &pos);

private:

    GLFWwindow *_win;
    const std::string _path;
    struct nk_context *_ctx;

    GameMap *_map;

    int _screenWidth;
    int _screenHeight;

    DrawingContext *_drawingContext;

    float _scale;
    xVec2 _direction;

    bool _mouseLocked;

    PFLayoutMode _layoutMode;
    PFPenMode _penMode;

    uint8_t _selectedTexture;
    uint8_t _selectedLayer;

    struct nk_image _layerImages[32];

    int _selectedImage;

    vector<string> _unitNames;
    vector<string> _baseNames;

    UNITTYPE _unit;
    int _baseTeam;

    vector<GameUnit *> _units;
    vector<GameBase *> _bases;

    bool _loading;
    bool _saving;
    bool _resizing;
    bool _cleaning;

    char mapsize[2][20];
    char mapname[128];

    bool _success;
    bool _error;
};
