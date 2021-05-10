#pragma once
#include "Actor.h"

class SoundObject : public Actor{
public:
    SoundObject(class Game* game);
    ~SoundObject();

    void UpdateActor(float deltaTime) override;

private:
    float duration;
    float speed;
    class MoveComponent* mvc;
};