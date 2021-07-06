#version 330
// ���ؽ� ���̴��κ����� �Է�
in vec2 fragTexCoord;	// �ؽ�ó ��ǥ
in vec3 fragNormal;		// ���� ���������� ����
in vec3 fragWorldPos;	// ���� �����󿡼��� ��ġ

// ���� ���̾ƿ� ��ȣ�� G ������ ��¿� �ش�
layout(location = 0) out vec3 outDiffuse;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outWorldPos;

// �л� ���� �ؽ�ó ���÷�
uniform sampler2D uTexture;

void main()
{
	// �л� ������ �ؽ�ó�κ��� ��´�
	outDiffuse = texture(uTexture, fragTexCoord).xyz;
	// ������ ���� ��ǥ�� �ٷ� G ���ۿ� ����
	outNormal = fragNormal;
	outWorldPos = fragWorldPos;
}