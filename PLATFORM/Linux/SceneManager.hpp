#pragma once

#include <string>
#include <map>

#include "BaseScene.hpp"

class SceneManager final {

public:

    SceneManager(struct nk_context *ctx, const std::string path);
    ~SceneManager() noexcept;

    SceneType Render(struct nk_font *small, struct nk_font *normal);

    bool setCurrent(const std::string & name);
	BaseScene* getScenePointer(const std::string & name);
    BaseScene* getCurrentScenePointer() const { return _currentScene; }

private:

    BaseScene*_currentScene;
    std::map<std::string, BaseScene*> _sceneDB;
};
