#include "PlaneActor.h"
#include "Game.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "BoxComponent.h"

PlaneActor::PlaneActor(Game* game)
    :Actor(game)
{
    SetScale(10.0f);
    MeshComponent* mc = new MeshComponent(this);
    Mesh* mesh = GetGame()->GetRenderer()->GetMesh("../Assets/Plane.gpmesh");
    mc->SetMesh(mesh);
    // Add collision box
    mBox = new BoxComponent(this);
    mBox->SetObjectBox(mesh->GetBox());

    game->AddPlane(this);
}

PlaneActor::~PlaneActor()
{
    GetGame()->RemovePlane(this);
}
