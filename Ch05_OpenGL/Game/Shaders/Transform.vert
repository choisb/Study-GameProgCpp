#version 330

// uniform�� in, out ������ �ٸ��� ���̴� ���α׷��� 
// ������ ȣ�� ���̿����� �����ϰ� �����Ǵ� ���� �����̴�.
// mat4 �� 3���� ������ǥ�� ��ȯ ����� 4x4 ����̴�.
uniform mat4 uWorldTransform;	
uniform mat4 uViewProj;

in vec3 inPosition;

void main()
{
	vec4 pos = vec4(inPosition, 1.0);
	gl_Position = pos * uWorldTransform * uViewProj;
}
