#include "SettingsScene.hpp"

SettingsScene::SettingsScene(const std::string & name, const std::string rootPath, struct nk_context *ctx) : BaseScene(name, rootPath, ctx) {

}

SettingsScene::~SettingsScene() {

}


SceneType SettingsScene::Render(struct nk_font *small, struct nk_font *normal) {

    return _type;
}

void SettingsScene::Init() {

}

void SettingsScene::Destroy() {

}

