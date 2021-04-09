// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Tower.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "Enemy.h"
#include "Bullet.h"
#include "AIComponent.h"
#include "AIState.h"

Tower::Tower(class Game* game)
:Actor(game)
{
    // mTowers 벡터에 등록
    game->GetTowers().emplace_back(this);
	SpriteComponent* sc = new SpriteComponent(this, 200);
	sc->SetTexture(game->GetTexture("../Assets/Tower.png"));
	
	mMove = new MoveComponent(this);
	//mMove->SetAngularSpeed(Math::Pi);
	
    // 인공지능 상태 등록
    AIComponent* aic = new AIComponent(this);
    aic->RegisterState(new AISearch(aic, this));
    aic->RegisterState(new AILockOn(aic, this));
    aic->RegisterState(new AIFire(aic, this));

    aic->ChangeState("Search");

	mNextAttack = AttackTime;
}

void Tower::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);
	
    // mNextAttack을 제외한기존코드 모두 삭제
	mNextAttack -= deltaTime;
}
