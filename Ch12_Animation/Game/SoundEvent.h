#pragma once
#include <string>
#include "Math.h"

class SoundEvent
{
public:
    SoundEvent();
    // ���� FMOD �̺�Ʈ �ν��Ͻ��� �����ϸ� true�� ��ȯ
    bool IsValid();
    // �̺�Ʈ�� ó������ �����
    void Restart();
    // �̺�Ʈ�� �����Ѵ�
    void Stop(bool allowFadeOut = true);
    // Setters
    void SetPaused(bool pause);
    void SetVolume(float value);
    void SetPitch(float value);
    void SetParameter(const std::string& name, float value);

    // Getters
    bool GetPaused() const;
    float GetVolume() const;
    float GetPitch() const;
    float GetParameter(const std::string& name);
    // ��ġ ��� �Լ�
    bool Is3D() const;
    void Set3DAttributes(const Matrix4& worldTrans);

protected:
    // �����ڸ� protected�� �����ϰ� AudioSystem������ ������ �� �ֵ��� 
    // firend class�� ����
    friend class AudioSystem;
    SoundEvent(class AudioSystem* system, unsigned int id);

private:
    class AudioSystem* mSystem;
    unsigned int mID;

};