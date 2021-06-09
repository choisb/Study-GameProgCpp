#include "SplineActor.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "SplineCamera.h"
#include "MoveComponent.h"

SplineActor::SplineActor(Game* game)
    :Actor(game)
{
    mCameraComp = new SplineCamera(this);

    // Create a spline
    Spline path;
    path.mControlPoints.emplace_back(Vector3::Zero);
    for (int i = 0; i < 5; i++)
    {
        if (i % 2 == 0)
        {
            path.mControlPoints.emplace_back(Vector3(300.0f * (i + 1), 0.0f, 300.0f));
        }
        else
        {
            path.mControlPoints.emplace_back(Vector3(300.0f * (i + 1), 0.0f, 0.0f));
        }
    }

    mCameraComp->SetSpline(path);
    mCameraComp->SetPaused(false);
}
void SplineActor::ActorInput(const uint8_t* keys)
{

}

void SplineActor::RestartSpline()
{
    mCameraComp->Restart();
}
