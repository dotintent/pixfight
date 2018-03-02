#include "SceneManager.hpp"

#include "MenuScene.hpp"
#include "NewGameScene.hpp"
#include "LoadGameScene.hpp"
#include "SettingsScene.hpp"
#include "RenderScene.hpp"

SceneManager::SceneManager(struct nk_context *ctx, const std::string path)
: _currentScene(nullptr) {

    auto menuScene = new MenuScene("menu", path, ctx);
    auto newGameScene = new NewGameScene("newgame", path, ctx);
    auto loadGameScene = new LoadGameScene("loadgame", path, ctx);
    auto settingsScene = new SettingsScene("settings", path, ctx);
    auto renderScene = new RenderScene("render", path, ctx);

    _sceneDB[menuScene->getName()] = menuScene;
    _sceneDB[newGameScene->getName()] = newGameScene;
    _sceneDB[loadGameScene->getName()] = loadGameScene;
    _sceneDB[settingsScene->getName()] = settingsScene;
    _sceneDB[renderScene->getName()] = renderScene;

	this->setCurrent("menu");
}

SceneManager::~SceneManager() noexcept {

    for (auto it = _sceneDB.begin(); it != _sceneDB.end(); it++) {

		if ((*it).second != nullptr) {

			(*it).second->Destroy();
			delete (*it).second;
		}

		(*it).second = nullptr;
	}

	_sceneDB.clear();
}

SceneType SceneManager::Render(struct nk_font *small, struct nk_font *normal) {

    return _currentScene->Render(small, normal);
}

bool SceneManager::setCurrent(const std::string & name) {

    auto it = _sceneDB.find(name);

	if (it == _sceneDB.end()) {

		return false;
    }

	if (_currentScene != it->second) {

		_currentScene = it->second;
	}

	return true;
}

BaseScene* SceneManager::getScenePointer(const std::string & name) {

    auto it = _sceneDB.find(name);

	if (it == _sceneDB.end()) {

		return nullptr;
    }

	return it->second;
}
