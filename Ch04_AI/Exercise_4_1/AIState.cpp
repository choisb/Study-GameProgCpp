// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "AIState.h"
#include "Game.h"
#include "Tower.h"
#include "Enemy.h"
#include "Bullet.h"
#include "AIComponent.h"
#include "Math.h"
#include "Actor.h"
#include "Game.h"
#include "Tile.h"
#include "Grid.h"
#include "CircleComponent.h"
#include "SDL/SDL_log.h"

// AIMove***************************************************
void AIMove::Update(float deltaTime)
{
//  SDL_Log("Updating %s state", GetName());
    Actor* actor = mOwner->GetActor();
    Vector2 diff = actor->GetPosition() - actor->GetGame()->GetGrid()->GetEndTile()->GetPosition();

    // 목적지에 도착했다면 AIDeath 상태로 전이
    if (Math::NearZero(diff.Length(), 10.0f))
    {
        mOwner->ChangeState("Death");
    }
}

void AIMove::OnEnter()
{
    //SDL_Log("Entering %s state", GetName());
}

void AIMove::OnExit()
{
    //SDL_Log("Exiting %s state", GetName());
}
//***********************************************************

// AIDeath***************************************************
void AIDeath::Update(float deltaTime)
{
    //SDL_Log("Updating %s state", GetName());
    Actor* actor = mOwner->GetActor();
    actor->SetState(Actor::EDead);
}

void AIDeath::OnEnter()
{
    //SDL_Log("Entering %s state", GetName());
}

void AIDeath::OnExit()
{
    //SDL_Log("Exiting %s state", GetName());
}

//***********************************************************

// AISearch****************************************************
void AISearch::Update(float deltaTime)
{
    // SDL_Log("Updating %s state", GetName());

    // 타겟을 탐색한다.
    Enemy* target = mTower->GetGame()->GetNearestEnemy(mTower->GetPosition());
    // 타겟을 발견했다면
    if (target != nullptr) {

        // 타겟과의 거리를 계산한다.
        Vector2 dir = target->GetPosition() - mTower->GetPosition();
        float dist = dir.Length();

        //  타겟이 사정거리 안에 들었다면 LockOn 상태로 전이
        if (dist < mTower->GetAttackRange()) {
            mTower->SetTarget(target);
            mOwner->ChangeState("LockOn");
        }
    }    
}

void AISearch::OnEnter()
{
    SDL_Log("Entering %s state", GetName());
}

void AISearch::OnExit()
{
    SDL_Log("Exiting %s state", GetName());
}

//***********************************************************

// AILockOn****************************************************
void AILockOn::Update(float deltaTime)
{
    // SDL_Log("Updating %s state", GetName());
    Enemy* target = mTower->GetTarget();
    // 타겟이 사라졌다면 Search 상태로 전이한다.
    if (target == nullptr)
        mOwner->ChangeState("Search");
    // 타겟이 존재할 경우
    else
    {
        // 타겟을 향해서 회전
        Vector2 dir = target->GetPosition() - mTower->GetPosition();
        mTower->SetRotation(Math::Atan2(-dir.y, dir.x));

        // 타겟과의 거리를 계산
        float dist = dir.Length();

        // 타겟이 사정거리를 벗어났다면 
        if (dist > mTower->GetAttackRange()) {
            mOwner->ChangeState("Search");
        }
        // 사거리 안에 있으면서, 쿨타임이 지났다면
        else if (mTower->GetNextAttack() < 0)
        {
            // Fire 상태로 전이
            mOwner->ChangeState("Fire");
        }

        // 사거리 안에 있지만 쿨타임일 경우 LockOn 상태 유지
    }
}

void AILockOn::OnEnter()
{
    SDL_Log("Entering %s state", GetName());
}

void AILockOn::OnExit()
{
    SDL_Log("Exiting %s state", GetName());
}

//***********************************************************

// AIFire****************************************************
void AIFire::Update(float deltaTime)
{
    // SDL_Log("Updating %s state", GetName());
    Enemy* target = mTower->GetTarget();
 
    // 타겟이 존재한다면 총알을 발사
    if (target != nullptr) {
        // 타워가 조준하고 있는 방향으로 bullet 생성
        Bullet* b = new Bullet(mTower->GetGame());
        b->SetPosition(mTower->GetPosition());
        b->SetRotation(mTower->GetRotation());
        // 쿨타임을 초기화 한다.
        mTower->SetNextAttack();
    }
    // 새로운 적군을 찾을 수 있도록 Search 상태로 전이
    mOwner->ChangeState("Search");
}

void AIFire::OnEnter()
{
    SDL_Log("Entering %s state", GetName());
}

void AIFire::OnExit()
{
    SDL_Log("Exiting %s state", GetName());
}
//***********************************************************

