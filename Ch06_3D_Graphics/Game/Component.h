#pragma once
#include <cstdint>
class Component {
public:
    // ������
    // (������Ʈ �������� ���� ���� ������Ʈ�� �� ���� ���ŵȴ�.)
    Component(class Actor* owner, int updateOrder = 100);
    // �Ҹ���
    virtual ~Component();
    // ��Ÿ �ð����� �� ������Ʈ�� ������Ʈ
	virtual void Update(float deltaTime) {}
    int GetUpdateOrder() const { return mUpdateOrder; }

	virtual void ProcessInput(const uint8_t* keyState) {}
    virtual void OnUpdateWorldTransform() {}

protected:
    // ������ ����, ������Ʈ�� �ʿ��� �� ���Ͱ� ���� �پ��� ������ �� ������ �����ϱ� ���� �ʿ�.
    class Actor* mOwner;
    // ������Ʈ�� ������Ʈ ����
    int mUpdateOrder;
};