// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

class AIState
{
public:
    AIState(class AIComponent * owner)
        :mOwner(owner)
    {}
    // �� ������ ��ü���� �ൿ
    virtual void Update(float deltaTime) = 0; // ���� ����
    virtual void OnEnter() = 0;     // ���°� ���۵� �� ȣ��
    virtual void OnExit() = 0;      // ���°� ����Ǳ� ���� ȣ��

    // ������ �̸� ���
    virtual const char* GetName() const = 0;
protected:
    class AIComponent* mOwner;
};

class AIMove : public AIState
{
public:
    AIMove(class AIComponent* owner)
		:AIState(owner)
	{ }

	// Override with behaviors for this state
	void Update(float deltaTime) override;
	void OnEnter() override;
	void OnExit() override;

	const char* GetName() const override
	{ return "Move"; }
};

class AIDeath : public AIState
{
public:
	AIDeath(class AIComponent* owner)
		:AIState(owner)
	{ }

	void Update(float deltaTime) override;
	void OnEnter() override;
	void OnExit() override;

	const char* GetName() const override
	{ return "Death"; }
};

class AISearch : public AIState
{
public:
    AISearch(class AIComponent* owner, class Tower* tower)
		:AIState(owner)
        ,mTower(tower)
	{ }

	void Update(float deltaTime) override;
	void OnEnter() override;
	void OnExit() override;

	const char* GetName() const override
	{ return "Search"; }

private:
    class Tower* mTower;
};

class AILockOn : public AIState
{
public:
    AILockOn(class AIComponent* owner, class Tower* tower)
        :AIState(owner)
        , mTower(tower)
    { }

    void Update(float deltaTime) override;
    void OnEnter() override;
    void OnExit() override;

    const char* GetName() const override
    {
        return "LockOn";
    }
private:
    class Tower* mTower;
};

class AIFire : public AIState
{
public:
    AIFire(class AIComponent* owner, class Tower* tower)
        :AIState(owner)
        , mTower(tower)
    { }

    void Update(float deltaTime) override;
    void OnEnter() override;
    void OnExit() override;

    const char* GetName() const override
    {
        return "Fire";
    }
private:
    class Tower* mTower;
};