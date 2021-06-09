#pragma once
#include <vector>
#include <string>

class Mesh
{
public:
    Mesh();
    ~Mesh();
    // �޽� �ε�/��ε�
    // renderer �����͸� �޴� ������ renderer�� ���ӿ� �ε�� �ؽ�ó�� ��� ���� �ְ�, MeshŬ������ �޽ÿ� ���õ� ��� �ؽ�ó�� ������ �� �ֵ��� �ϱ� ����.
    bool Load(const std::string& fileName, class Renderer* renderer);
    void Unload();
    // �� �޽ÿ� �ش��ϴ� ���ؽ� �迭�� ��´�.
    class VertexArray* GetVertexArray() { return mVertexArray; }
    // Ư�� �ε����� �ش��ϴ� �ؽ�ó�� ��´�.
    class Texture* GetTexture(size_t index);
    // ���̴��� �̸��� ��´�.
    const std::string& GetShaderName() const { return mShaderName; }
    // ������Ʈ ���� �ٿ�� ��ü �������� ��´�.
    float GetRadius() const { return mRadius; }
    // ���ݻ� ���� ��ȯ
    float GetSpecPower() const { return mSpecPower; }

private:

    // �� �޽ÿ� ���Ǵ� �ؽ�ó��
    std::vector<class Texture*> mTextures;
    // �޽��� ���ؽ� �迭
    class VertexArray* mVertexArray;
    // �޽ð� ������ ���̴� �̸�
    std::string mShaderName;
    // ������Ʈ ���� �ٿ�� ��ü�� �������� ����. �浹 �˻翡�� Ȱ���� �� �ִ�.
    // �������� ���� �ָ� ������ �������� �Ÿ����� �ٿ�� ��ü�� ���������� ���.
    float mRadius;
    // ���ݻ� ����
    float mSpecPower;
};