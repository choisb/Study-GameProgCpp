#pragma once
#include "Math.h"
#include <vector>   
class Actor
{
public:
    // ������ ���¸� �����ϴµ� ���ȴ�.
    enum State
    {
        EActive,
        EPaused,
        EDead
    };
    // ������ ����(dependency injection). actor �����ڰ� Game Ŭ������ �����͸� �޴´�.
    // Actor�� �ٸ� Actor�� �����ϰų� Game �Լ��� �����ϱ� ���� �� �����͸� ����� �� �ִ�.
    Actor(class Game* game);
    virtual ~Actor();

    // Game���� ȣ���ϴ� Update �Լ� (���� �Լ� �ƴ� ����)
    void Update(float deltaTime);
    // ���Ϳ� ������ ��� ������Ʈ�� ������Ʈ (���� �Լ� �ƴ� ����)
    void UpdateComponents(float deltaTime);
    // Ư�� ���Ϳ� Ưȭ�� ������Ʈ �ڵ� (�������̵� ����)
    virtual void UpdateActor(float deltaTime);

    // Getters/setters
    const Vector3& GetPosition() const { return mPosition; }
    void SetPosition(const Vector3& pos) { mPosition = pos; mRecomputeWorldTransform = true; }
    float GetScale() const { return mScale; }
    void SetScale(float scale) { mScale = scale; mRecomputeWorldTransform = true; }
    Quaternion GetRotation() const { return mRotation; }
    void SetRotation(const Quaternion& rotation) { mRotation = rotation; mRecomputeWorldTransform = true; }

    void ComputeWorldTransform();
    const Matrix4& GetWorldTransform() const { return mWorldTransform; }

    State GetState() const { return mState; }
    void SetState(State state) { mState = state; }

    Vector3 GetForward() const { return Vector3::Transform(Vector3::UnitX, mRotation); }

    class Game* GetGame() { return mGame; }

    float GetForwardSpeed() const { return mForwardSpeed; }

    // ������Ʈ �߰�/����
    void AddComponent(class Component* component);
    void RemoveComponent(class Component* component);

	void ProcessInput(const uint8_t* keyState);
	virtual void ActorInput(const uint8_t* keyState) {}

protected:
    float mForwardSpeed;

private:
    // ������ ����
    State mState;

    // ��ȯ
    Matrix4 mWorldTransform;
    Vector3 mPosition;  // ������ �߽���
    float mScale;       // ������ ���� (100%�� ��� 1.0f)
    Quaternion mRotation;    // ȸ�� ���� (����)
    bool mRecomputeWorldTransform;

    // �� ���Ͱ� ������ ������Ʈ��
    std::vector<class Component*> mComponents;

    // ���Ͱ� ���� Game ��ü�� �ּ�
    class Game* mGame;


};