#include "LoadGameScene.hpp"

LoadGameScene::LoadGameScene(const std::string & name, const std::string rootPath, struct nk_context *ctx) : BaseScene(name, rootPath, ctx) {

}

LoadGameScene::~LoadGameScene() {

}


SceneType LoadGameScene::Render(struct nk_font *small, struct nk_font *normal) {

    return _type;
}

void LoadGameScene::Init() {

}

void LoadGameScene::Destroy() {

}

