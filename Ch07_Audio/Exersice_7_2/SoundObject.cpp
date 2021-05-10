#include "SoundObject.h"
#include "MoveComponent.h"
#include "MeshComponent.h"
#include "AudioComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"

SoundObject::SoundObject(class Game* game)
    :Actor(game)
    , duration(3.f)
    , speed(-300.f)
{
    SetPosition(Vector3(500.f, -75.f, 0.f));
    SetScale(1.f);

    MeshComponent* msc;
    msc = new MeshComponent(this);
    msc->SetMesh(GetGame()->GetRenderer()->GetMesh("../Assets/Sphere.gpmesh"));

    AudioComponent* ac = new AudioComponent(this);
    ac->PlayEvent("event:/FireLoop");

    mvc = new MoveComponent(this);
    mvc->SetForwardSpeed(speed);
}
SoundObject::~SoundObject()
{

}

void SoundObject::UpdateActor(float deltaTime)
{
    duration -= deltaTime;
    if (duration < 0.f)
    {
        duration = 3.f;
        speed = -1 * speed;
        mvc->SetForwardSpeed(speed);
    }
}
