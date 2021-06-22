#pragma once
#include "UIScreen.h"
#include <vector>

class HUD : public UIScreen
{
public:
    HUD(class Game* game);
    ~HUD();

    void Update(float deltaTime) override;
    //void Draw(class Shader* shader) override;

protected:
};