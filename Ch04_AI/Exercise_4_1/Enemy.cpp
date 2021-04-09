// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Enemy.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "NavComponent.h"
#include "Grid.h"
#include "Tile.h"
#include "CircleComponent.h"
#include "AIComponent.h"
#include "AIState.h"
#include "Tower.h"
#include <algorithm>

Enemy::Enemy(class Game* game)
:Actor(game)
{
    // Add to enemy vector
    game->GetEnemies().emplace_back(this);
	
    SpriteComponent* sc = new SpriteComponent(this);
    sc->SetTexture(game->GetTexture("../Assets/Airplane.png"));
    // Set position at start tile
    SetPosition(GetGame()->GetGrid()->GetStartTile()->GetPosition());
    // Setup a nav component at the start tile
    NavComponent* nc = new NavComponent(this);
    nc->SetForwardSpeed(150.0f);
    nc->StartPath(GetGame()->GetGrid()->GetStartTile());
    // Setup a circle for collision
    mCircle = new CircleComponent(this);
    mCircle->SetRadius(25.0f);

    // 인공지능 상태 등록
    AIComponent* aic = new AIComponent(this);
    aic->RegisterState(new AIMove(aic));
    aic->RegisterState(new AIDeath(aic));

    aic->ChangeState("Move");
}

Enemy::~Enemy()
{
	// Remove from enemy vector
	auto iter = std::find(GetGame()->GetEnemies().begin(),
						  GetGame()->GetEnemies().end(),
						  this);
	GetGame()->GetEnemies().erase(iter);

    // 모든 타워 인스턴스에서 
    // 삭제할 this(적군)를 타겟으로 가진 타워는 타겟을 nullptr로 설정 
    std::for_each(GetGame()->GetTowers().begin(),
                GetGame()->GetTowers().end(),
                [this](Tower* tower) {
                    if (tower->GetTarget() == this)
                        tower->SetTarget(nullptr);
                }); // for_each의 3번자 인자를 람다식으로 사용
}

void Enemy::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);
    // 기존의 코드 제거
}
