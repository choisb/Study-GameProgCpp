#include "HUD.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"
#include "Renderer.h"
#include "PhysWorld.h"
#include "FPSActor.h"
#include <algorithm>

HUD::HUD(Game* game)
    :UIScreen(game)
{
}
HUD::~HUD()
{

}

void HUD::Update(float deltaTime)
{
    UIScreen::Update(deltaTime);

}
