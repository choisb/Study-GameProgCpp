#include "Component.h"
#include "Actor.h"

Component::Component(Actor* owner, int updateOrder = 100) 
    : mUpdateOrder(updateOrder)
    , mOwner(owner)
{
    mOwner->AddComponent(this);
}
Component::~Component()
{
    mOwner->RemoveComponent(this);
}
void Component::Update(float deltaTime)
{

}
