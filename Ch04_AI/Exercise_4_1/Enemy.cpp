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

    // �ΰ����� ���� ���
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

    // ��� Ÿ�� �ν��Ͻ����� 
    // ������ this(����)�� Ÿ������ ���� Ÿ���� Ÿ���� nullptr�� ���� 
    std::for_each(GetGame()->GetTowers().begin(),
                GetGame()->GetTowers().end(),
                [this](Tower* tower) {
                    if (tower->GetTarget() == this)
                        tower->SetTarget(nullptr);
                }); // for_each�� 3���� ���ڸ� ���ٽ����� ���
}

void Enemy::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);
    // ������ �ڵ� ����
}
