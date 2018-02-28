#include "RenderScene.hpp"

RenderScene::RenderScene(const std::string & name, const std::string rootPath, struct nk_context *ctx) : BaseScene(name, rootPath, ctx) {

}

RenderScene::~RenderScene() {

}


SceneType RenderScene::Render() {

    return _type;
}

void RenderScene::Init() {

}

void RenderScene::Destroy() {

}

