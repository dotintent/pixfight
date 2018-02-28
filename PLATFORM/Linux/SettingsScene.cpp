#include "SettingsScene.hpp"

SettingsScene::SettingsScene(const std::string & name, const std::string rootPath, struct nk_context *ctx) : BaseScene(name, rootPath, ctx) {

}

SettingsScene::~SettingsScene() {

}


SceneType SettingsScene::Render() {

    return _type;
}

void SettingsScene::Init() {

}

void SettingsScene::Destroy() {

}

