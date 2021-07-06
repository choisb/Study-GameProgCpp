#include "HUD.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"
#include "Renderer.h"
#include "PhysWorld.h"
#include "FollowActor.h"
#include "TargetComponent.h"
#include "MirrorCamera.h"
#include <algorithm>

HUD::HUD(Game* game)
    :UIScreen(game)
    ,mTargetEnemy(false)
    ,mRadarRadius(92.0f)
    ,mRadarRange(2000.0f)
{
    Renderer* r = mGame->GetRenderer();
    mRadar = r->GetTexture("../Assets/Radar.png");
    mBlipTex = r->GetTexture("../Assets/Blip.png");
    mRadarArrow = r->GetTexture("../Assets/RadarArrow.png");
    mHealthBar = r->GetTexture("../Assets/HealthBar.png");
}
HUD::~HUD()
{

}

void HUD::Update(float deltaTime)
{
    UIScreen::Update(deltaTime);

    UpdateCrosshair(deltaTime);
    UpdateRadar(deltaTime);

}
void HUD::Draw(Shader* shader)
{

    // Radar
    const Vector2 cRadarPos(-390.0f, 275.0f);
    DrawTexture(shader, mRadar, cRadarPos, 1.0f);

    // Blips
    for (Vector2& blip : mBlips)
    {
        DrawTexture(shader, mBlipTex, cRadarPos + blip, 1.0f);
    }

    // Radar arrow
    DrawTexture(shader, mRadarArrow, cRadarPos);

    Texture* mirror = mGame->GetRenderer()->GetMirrorTexture();
    DrawTexture(shader, mirror, Vector2(-350.0f, -250.0f), 1.0f, true);

}
void HUD::AddTargetComponent(TargetComponent* tc)
{
    mTargetComps.emplace_back(tc);
}
void HUD::RemoveTargetComponent(TargetComponent* tc)
{
    auto iter = std::find(mTargetComps.begin(), mTargetComps.end(), tc);
    mTargetComps.erase(iter);
}
void HUD::UpdateCrosshair(float deltaTime)
{
    // 일반 커서로 리셋
    mTargetEnemy = false;
    // 선분을 만든다.
    // 선분의 길이(유효 사거리)
    const float cAimDist = 5000.0f;
    Vector3 start, dir;
    mGame->GetRenderer()->GetScreenDirection(start, dir);
    LineSegment l(start, start + dir * cAimDist);
    // 선분 캐스트
    PhysWorld::CollisionInfo info;
    if (mGame->GetPhysWorld()->SegmentCast(l, info))
    {
        // SegmentCast를 통해 얻은 액터가 타깃 컴포넌트의 소유자와 일치하는지를 체크
        for (auto tc : mTargetComps)
        {
            if (tc->GetOwner() == info.mActor)
            {
                mTargetEnemy = true;
                break;
            }
        }
    }
}
void HUD::UpdateRadar(float deltaTime)
{
    // 신호 오프셋 벡터 클리어
    mBlips.clear();

    // 플레이어 위치를 레이더 좌표로 변환
    // 3D에서는 x축이 정방
    // 2D에서는 y축이 정방
    // 때문에 (x, y, z) 좌표를 (y, x) 좌표로 변경
    Vector3 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 playerPos2D(playerPos.y, playerPos.x);

    // 레이더와 전방은 플레이어의 전방과같음
    Vector3 playerForward = mGame->GetPlayer()->GetForward();
    Vector2 playerForward2D(playerForward.x, playerForward.y);

    // 레이더를 회전시키기 위해 atan2 함수 사용
    float angle = Math::Atan2(playerForward2D.y, playerForward2D.x);
    // 2D 회전 행렬을 만든다.
    Matrix3 rotMat = Matrix3::CreateRotation(angle);

    // 오브젝트 신호의 위치를 얻는다
    for (auto tc : mTargetComps)
    {
        Vector3 targetPos = tc->GetOwner()->GetPosition();
        Vector2 actorPos2D(targetPos.y, targetPos.x);

        // 플레이어와 타깃 사이의 벡터를 계산
        Vector2 playerToTarget = actorPos2D - playerPos2D;

        // 타깃이 범위 안에 있다면
        if (playerToTarget.LengthSq() <= (mRadarRange * mRadarRange))
        {
            // playerToTarget 좌표를
            // 레이더 화면 중심으로부터 오프셋으로 변환
            Vector2 blipPos = playerToTarget;
            blipPos *= mRadarRadius / mRadarRange;
            // 신호를 회전시켜 레이더 공간의 최종 좌표로 변환
            blipPos = Vector2::Transform(blipPos, rotMat);
            mBlips.emplace_back(blipPos);
        }
    }
}