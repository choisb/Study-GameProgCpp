#pragma once
#include "Actor.h"

class Ship : public Actor
{
public:
    Ship(class Game* game);
    void UpdateActor(float deltaTime) override;
	void ActorInput(const uint8_t* keyState) override;

private:
	float mLaserCooldown;
	class CircleComponent* mCircle;
	// 리스폰 시간
	float mRespawnTime;
	// 부활후 무적 시간.
	float mInvincibleTime;

};