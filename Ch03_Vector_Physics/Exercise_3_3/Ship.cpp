#pragma once
#include "Ship.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "AnimSpriteComponent.h"

Ship::Ship(Game* game)
    : Actor(game)
{
	AnimSpriteComponent* asc = new AnimSpriteComponent(this);
	std::vector<SDL_Texture*> anims = {
		game->GetTexture("../Assets/Ship01.png"),
		game->GetTexture("../Assets/Ship02.png"),
		game->GetTexture("../Assets/Ship03.png"),
		game->GetTexture("../Assets/Ship04.png")
	};
	asc->SetAnimTextures(anims);

	// Create InputComponent and set keys/force
	InputComponent* ic = new InputComponent(this);
	ic->SetFireKey(SDL_SCANCODE_SPACE);
	ic->SetFireForce(Vector2(15.0f,-200.0f));
	ic->SetMass(10.0f);
}

void Ship::UpdateActor(float deltaTime)
{
}

void Ship::ActorInput(const uint8_t* keyState)
{
}
