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

    // �������� �����ߴٸ� AIDeath ���·� ����
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

    // Ÿ���� Ž���Ѵ�.
    Enemy* target = mTower->GetGame()->GetNearestEnemy(mTower->GetPosition());
    // Ÿ���� �߰��ߴٸ�
    if (target != nullptr) {

        // Ÿ�ٰ��� �Ÿ��� ����Ѵ�.
        Vector2 dir = target->GetPosition() - mTower->GetPosition();
        float dist = dir.Length();

        //  Ÿ���� �����Ÿ� �ȿ� ����ٸ� LockOn ���·� ����
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
    // Ÿ���� ������ٸ� Search ���·� �����Ѵ�.
    if (target == nullptr)
        mOwner->ChangeState("Search");
    // Ÿ���� ������ ���
    else
    {
        // Ÿ���� ���ؼ� ȸ��
        Vector2 dir = target->GetPosition() - mTower->GetPosition();
        mTower->SetRotation(Math::Atan2(-dir.y, dir.x));

        // Ÿ�ٰ��� �Ÿ��� ���
        float dist = dir.Length();

        // Ÿ���� �����Ÿ��� ����ٸ� 
        if (dist > mTower->GetAttackRange()) {
            mOwner->ChangeState("Search");
        }
        // ��Ÿ� �ȿ� �����鼭, ��Ÿ���� �����ٸ�
        else if (mTower->GetNextAttack() < 0)
        {
            // Fire ���·� ����
            mOwner->ChangeState("Fire");
        }

        // ��Ÿ� �ȿ� ������ ��Ÿ���� ��� LockOn ���� ����
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
 
    // Ÿ���� �����Ѵٸ� �Ѿ��� �߻�
    if (target != nullptr) {
        // Ÿ���� �����ϰ� �ִ� �������� bullet ����
        Bullet* b = new Bullet(mTower->GetGame());
        b->SetPosition(mTower->GetPosition());
        b->SetRotation(mTower->GetRotation());
        // ��Ÿ���� �ʱ�ȭ �Ѵ�.
        mTower->SetNextAttack();
    }
    // ���ο� ������ ã�� �� �ֵ��� Search ���·� ����
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

