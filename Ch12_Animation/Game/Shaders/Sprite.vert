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

void main()
{
	// ��ġ�� ���� ��ǥ�� ��ȯ
	vec4 pos = vec4(inPosition, 1.0);
	// ��ġ�� ���� �������� ��ȯ�� �� Ŭ�� �������� ��ȯ
	gl_Position = pos * uWorldTransform * uViewProj;
	// �ؽ�ó ��ǥ�� �����׸�Ʈ ���̴��� ����
	fragTexCoord = inTexCoord;
}
