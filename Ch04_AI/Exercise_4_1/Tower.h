// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Actor.h"

class Tower : public Actor
{
public:
    Tower(class Game* game);
    void UpdateActor(float deltaTime) override;
    // Å¸°Ù Getter / Setter
    void SetTarget(class Enemy* target) { mTarget = target; }
    class Enemy* GetTarget(){ return mTarget; }
    float GetNextAttack() { return mNextAttack; }
    void SetNextAttack() { mNextAttack = AttackTime; }// ÄðÅ¸ÀÓ ÃÊ±âÈ­
    float GetAttackRange() { return AttackRange; }

private:
    class MoveComponent* mMove;
    float mNextAttack;
    const float AttackTime = 2.5f;
    const float AttackRange = 100.0f;
    class Enemy* mTarget;   // ¸â¹öº¯¼ö Å¸°Ù Ãß°¡
};
