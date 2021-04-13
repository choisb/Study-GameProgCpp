#pragma once
#include "Math.h"
#include <vector>   
class Actor
{
public:
    // 액터의 상태를 추적하는데 사용된다.
    enum State
    {
        EActive,
        EPaused,
        EDead
    };
    // 의존성 주입(dependency injection). actor 생성자가 Game 클래스의 포인터를 받는다.
    // Actor는 다른 Actor를 생성하거나 Game 함수에 접근하기 위햏 이 포인터를 사용할 수 있다.
    Actor(class Game* game);
    virtual ~Actor();

    // Game에서 호출하는 Update 함수 (가상 함수 아님 주의)
    void Update(float deltaTime);
    // 액터에 부착된 모든 컴포넌트를 업데이트 (가상 함수 아님 주의)
    void UpdateComponents(float deltaTime);
    // 특정 액터에 특화된 업데이트 코드 (오버라이딩 가능)
    virtual void UpdateActor(float deltaTime);

    // Getters/setters
    const Vector2& GetPosition() const { return mPosition; }
    void SetPosition(const Vector2& pos) { mPosition = pos; mRecomputeWorldTransform = true; }
    float GetScale() const { return mScale; }
    void SetScale(float scale) { mScale = scale; mRecomputeWorldTransform = true; }
    float GetRotation() const { return mRotation; }
    void SetRotation(float rotation) { mRotation = rotation; mRecomputeWorldTransform = true; }

    void ComputeWorldTransform();
    const Matrix4& GetWorldTransform() const { return mWorldTransform; }

    State GetState() const { return mState; }
    void SetState(State state) { mState = state; }
	//SDL은 아래쪽 방향이 +y방향이기 때문에 y값에 (-)를 곱해야함.
	Vector2 GetForward() const { return Vector2(Math::Cos(mRotation), -Math::Sin(mRotation)); }

    class Game* GetGame() { return mGame; }

    // 컴포넌트 추가/제거
    void AddComponent(class Component* component);
    void RemoveComponent(class Component* component);

	void ProcessInput(const uint8_t* keyState);
	virtual void ActorInput(const uint8_t* keyState) {}

private:
    // 액터의 상태
    State mState;

    // 변환
    Matrix4 mWorldTransform;
    Vector2 mPosition;  // 액터의 중심점
    float mScale;       // 액터의 배율 (100%의 경우 1.0f)
    float mRotation;    // 회전 각도 (라디안)
    bool mRecomputeWorldTransform;

    // 이 액터가 보유한 컴포넌트들
    std::vector<class Component*> mComponents;

    // 액터가 속한 Game 객체의 주소
    class Game* mGame;
};