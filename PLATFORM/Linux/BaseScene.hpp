#pragma once

#include "Core-pch.hpp"
#include "EnumOptionMacro.hpp"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING

#include "nuklear.h"

typedef PF_ENUM(unsigned int, SceneType) {

    SceneTypeMenu,
    SceneTypeNewGame,
    SceneTypeLoadGame,
    SceneTypeSettings,
    SceneTypeRender,
    SceneTypeNone
};

class BaseScene {

public:

    BaseScene(const std::string & name, const std::string rootPath, struct nk_context *ctx)
    : _name(name)
    , _rootPath(rootPath)
    , _ctx(ctx)
    , _type(SceneTypeNone) {}

    virtual ~BaseScene() noexcept = default;

    BaseScene(const BaseScene & other) = delete;
    BaseScene(BaseScene && other) noexcept = delete;

    BaseScene & operator= (const BaseScene & other) = delete;
    BaseScene & operator= (BaseScene && other) noexcept = delete;

    std::string const & getName() const { return _name; }

    virtual SceneType Render(struct nk_font *smallfont, struct nk_font *normal) = 0;

    virtual void Init() = 0;
    virtual void Destroy() = 0;

    virtual bool isRender() { return false; }

    virtual void handleScroll(double s) {}
    virtual void handleMouse(int button, int action, double x, double y) {}
    virtual void handleMove(const xVec2 &direction) {}

protected:

    std::string _name;
    std::string _rootPath;
    struct nk_context *_ctx;
    SceneType _type;
};

