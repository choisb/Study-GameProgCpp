#pragma once
#include "Ship.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "InputComponent.h"

Ship::Ship(Game* game)
    : Actor(game)
{
	SpriteComponent* sc = new SpriteComponent(this, 150);
	sc->SetTexture(game->GetTexture("../Assets/Ship.png"));

	// Create InputComponent and set keys/speed
	InputComponent* ic = new InputComponent(this);
	ic->SetForwardKey(SDL_SCANCODE_W);
	ic->SetBackKey(SDL_SCANCODE_S);
	ic->SetClockwiseKey(SDL_SCANCODE_A);
	ic->SetCounterClockwiseKey(SDL_SCANCODE_D);
	ic->SetMaxForwardSpeed(300.0f);
	ic->SetMaxAngularSpeed(Math::TwoPi);
}

void Ship::UpdateActor(float deltaTime)
{

}

void Ship::ActorInput(const uint8_t* keyState)
{

}
