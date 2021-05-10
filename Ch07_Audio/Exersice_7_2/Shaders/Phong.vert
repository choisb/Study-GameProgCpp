#version 330

// uniform�� in, out ������ �ٸ��� ���̴� ���α׷��� 
// ������ ȣ�� ���̿����� �����ϰ� �����Ǵ� ���� �����̴�.
// mat4 �� 3���� ������ǥ�� ��ȯ ����� 4x4 ����̴�.
uniform mat4 uWorldTransform;	
uniform mat4 uViewProj;

// layout ��ɾ�� �Ӽ� ������ � ������ �ش��ϴ��� ������ �� �ִ�.
// ���⼭ �Ӽ� �����̶� glVertexAttribPointer �Լ��� ȣ������ ���� ���� ��ȣ�̴�.
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// ���ؽ� ���̴��� �Էµ� �ؽ��� ��ǥ�� �����׸�Ʈ ���̴��� �����ϱ� ���� ����
// out ������ �����ϸ� ���ؽ� ���̴����� �����׸�Ʈ ���̴��� ������ ������ �����ϴ�.
out vec2 fragTexCoord;
// ���� ���������� ����
out vec3 fragNormal;
// ���� ���������� ��ġ
out vec3 fragWorldPos;

void main()
{
	// ��ġ�� ���� ��ǥ�� ��ȯ
	vec4 pos = vec4(inPosition, 1.0);
	// ��ġ�� ���� �������� ��ȯ�� �� Ŭ�� �������� ��ȯ
	pos = pos * uWorldTransform;
	// ��ġ�� Ŭ�� ���� ��ǥ�� ��ȯ
	gl_Position = pos * uViewProj;

	// �����׸�Ʈ ���̴��� ������ ���� �κ�
	// ���� �������� ��ġ�� ����
	fragWorldPos = pos.xyz;
	// �������͸� ���� �������� �������� ��ȯ
	// �������ʹ� ������̱⿡ �̵���ȯ�� �ǹ� ����. ������ ���� ��ǥ�迡�� w=0 �̴�
	fragNormal = (vec4(inNormal, 0.0f) * uWorldTransform).xyz;

	// �ؽ�ó ��ǥ�� �����׸�Ʈ ���̴��� ����
	fragTexCoord = inTexCoord;
}
