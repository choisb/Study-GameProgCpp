#include "TargetComponent.h"
#include "Game.h"
#include "HUD.h"
#include "Actor.h"

TargetComponent::TargetComponent(Actor *owner)
    :Component(owner)
{
    mOwner->GetGame()->GetHUD()->AddTargetComponent(this);
}
TargetComponent::~TargetComponent()
{
    mOwner->GetGame()->GetHUD()->RemoveTargetComponent(this);
}